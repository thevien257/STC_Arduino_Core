#!/bin/bash
# SDCC Linker Wrapper Script - Smart Driver Linking
# Only links driver files that are actually called

SDCC="$1"
shift
FLAGS=""
REL_FILES=""
DRIVER_FILES=""
CLEANUP_FILES=""

# Parse arguments and categorize files
while [ $# -gt 0 ]; do
    ARG="$1"
    case "$ARG" in
        *.o)
            REL_FILE="${ARG%.o}.rel"
            if [ -f "$ARG" ] && [ ! -f "$REL_FILE" ]; then
                cp "$ARG" "$REL_FILE"
                CLEANUP_FILES="$CLEANUP_FILES $REL_FILE"
            fi
            
            if [ -f "$REL_FILE" ]; then
                # Separate driver files from regular files
                if [[ "$ARG" == */drivers/src/* ]]; then
                    DRIVER_FILES="$DRIVER_FILES $REL_FILE"
                else
                    REL_FILES="$REL_FILES $REL_FILE"
                fi
            fi
            ;;
        *.a)
            LIB_FILE="${ARG%.a}.lib"
            if [ -f "$ARG" ] && [ ! -f "$LIB_FILE" ]; then
                cp "$ARG" "$LIB_FILE"
                CLEANUP_FILES="$CLEANUP_FILES $LIB_FILE"
            fi
            if [ -f "$LIB_FILE" ]; then
                REL_FILES="$REL_FILES $LIB_FILE"
            fi
            ;;
        -o)
            shift
            OUTPUT="$1"
            FLAGS="$FLAGS -o $OUTPUT"
            ;;
        *)
            FLAGS="$FLAGS $ARG"
            ;;
    esac
    shift
done

# Iteratively resolve undefined symbols by adding needed drivers
NEEDED_DRIVERS=""
MAX_ITERATIONS=10

for iteration in $(seq 1 $MAX_ITERATIONS); do
    # Try linking with current set
    $SDCC $FLAGS $REL_FILES $NEEDED_DRIVERS > /tmp/sdcc_link_$.txt 2>&1
    LINK_RESULT=$?
    
    # If link succeeded, we're done
    if [ $LINK_RESULT -eq 0 ]; then
        break
    fi
    
    # Extract undefined symbol names from error output
    UNDEFINED=$(grep -oP "(?<=Undefined Global '_)[^']*" /tmp/sdcc_link_$.txt 2>/dev/null | head -1)
    
    # If no undefined symbols found, stop
    if [ -z "$UNDEFINED" ]; then
        break
    fi
    
    # Search for which driver file provides this symbol
    FOUND_DRIVER=""
    for DRIVER in $DRIVER_FILES; do
        # Skip if already added
        if echo "$NEEDED_DRIVERS" | grep -q "$DRIVER"; then
            continue
        fi
        
        # Check if this driver defines the symbol
        if grep -q "^S _${UNDEFINED} Def" "$DRIVER" 2>/dev/null; then
            FOUND_DRIVER="$DRIVER"
            break
        fi
    done
    
    # If we found the driver, add it
    if [ -n "$FOUND_DRIVER" ]; then
        NEEDED_DRIVERS="$NEEDED_DRIVERS $FOUND_DRIVER"
    else
        # No driver provides this symbol, stop trying
        break
    fi
done

# Final link with all needed drivers
$SDCC $FLAGS $REL_FILES $NEEDED_DRIVERS
RESULT=$?

# Cleanup
for FILE in $CLEANUP_FILES; do
    rm -f "$FILE"
done
rm -f /tmp/sdcc_link_$.txt

exit $RESULT
