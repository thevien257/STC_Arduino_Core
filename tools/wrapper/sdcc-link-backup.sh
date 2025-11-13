#!/bin/bash
# SDCC Linker Wrapper Script for Arduino with Smart Driver Linking
# Only links driver files that are actually needed

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
            # Check if this is a driver file
            if [[ "$ARG" == */drivers/src/* ]]; then
                # This is a driver file - add to separate list
                REL_FILE="${ARG%.o}.rel"
                if [ -f "$ARG" ] && [ ! -f "$REL_FILE" ]; then
                    cp "$ARG" "$REL_FILE"
                    CLEANUP_FILES="$CLEANUP_FILES $REL_FILE"
                fi
                if [ -f "$REL_FILE" ]; then
                    DRIVER_FILES="$DRIVER_FILES $REL_FILE"
                fi
            else
                # Regular file - always include
                REL_FILE="${ARG%.o}.rel"
                if [ -f "$ARG" ] && [ ! -f "$REL_FILE" ]; then
                    cp "$ARG" "$REL_FILE"
                    CLEANUP_FILES="$CLEANUP_FILES $REL_FILE"
                fi
                if [ -f "$REL_FILE" ]; then
                    REL_FILES="$REL_FILES $REL_FILE"
                fi
            fi
            ;;
        *.a)
            # Convert .a to .lib
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

# First pass: Try linking WITHOUT driver files
# >&2 echo "Attempting minimal link..."
$SDCC $FLAGS $REL_FILES 2>&1 | tee /tmp/sdcc_link_errors.txt
RESULT=$?

# Check if there are undefined symbol warnings
if grep -q "Undefined Global" /tmp/sdcc_link_errors.txt 2>/dev/null; then
    # >&2 echo "Found undefined symbols, analyzing..."
    
    # Check which driver functions are needed
    NEEDED_DRIVERS=""
    ADD_GPIO=0
    ADD_TIMER=0
    ADD_CLOCK=0
    
    # Read the error file and check for specific function patterns
    while IFS= read -r line; do
        if [[ "$line" == *"Undefined Global"* ]]; then
            # GPIO functions
            if [[ "$line" =~ pin|Pin|gpio|digital|Digital|analog|Analog ]]; then
                ADD_GPIO=1
            fi
            # Timer/delay functions
            if [[ "$line" =~ delay|millis|micros|timer|Timer ]]; then
                ADD_TIMER=1
            fi
            # Clock functions
            if [[ "$line" =~ clock|Clock ]]; then
                ADD_CLOCK=1
            fi
        fi
    done < /tmp/sdcc_link_errors.txt
    
    # Add the needed drivers
    for DRIVER in $DRIVER_FILES; do
        if [[ "$DRIVER" == *gpio* ]] && [ $ADD_GPIO -eq 1 ]; then
            NEEDED_DRIVERS="$NEEDED_DRIVERS $DRIVER"
            # >&2 echo "  Adding driver: $(basename $DRIVER)"
        fi
        if [[ "$DRIVER" == *timer* ]] && [ $ADD_TIMER -eq 1 ]; then
            NEEDED_DRIVERS="$NEEDED_DRIVERS $DRIVER"
            # >&2 echo "  Adding driver: $(basename $DRIVER)"
        fi
        if [[ "$DRIVER" == *clock* ]] && [ $ADD_CLOCK -eq 1 ]; then
            NEEDED_DRIVERS="$NEEDED_DRIVERS $DRIVER"
            # >&2 echo "  Adding driver: $(basename $DRIVER)"
        fi
    done
    
    # If we found needed drivers, retry with them
    if [ -n "$NEEDED_DRIVERS" ]; then
        # >&2 echo "Re-linking with required drivers..."
        $SDCC $FLAGS $REL_FILES $NEEDED_DRIVERS
        RESULT=$?
    else
        >&2 echo "Warning: Undefined symbols but no matching drivers found!"
        >&2 echo "Adding ALL drivers as fallback..."
        $SDCC $FLAGS $REL_FILES $DRIVER_FILES
        RESULT=$?
    fi
else
    >&2 echo "Minimal link succeeded - no drivers needed!"
fi

# Cleanup temporary files
for FILE in $CLEANUP_FILES; do
    rm -f "$FILE"
done
rm -f /tmp/sdcc_link_errors.txt

exit $RESULT
