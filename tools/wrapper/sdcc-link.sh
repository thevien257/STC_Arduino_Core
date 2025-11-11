#!/bin/bash
# SDCC Linker Wrapper Script for Arduino
# Handles conversion of .o and .a files to .rel and .lib for linking

SDCC="$1"
shift
FLAGS=""
REL_FILES=""
CLEANUP_FILES=""

# Parse arguments and convert file extensions
while [ $# -gt 0 ]; do
    ARG="$1"
    case "$ARG" in
        *.o)
            # Convert .o to .rel
            REL_FILE="${ARG%.o}.rel"
            if [ -f "$ARG" ] && [ ! -f "$REL_FILE" ]; then
                cp "$ARG" "$REL_FILE"
                CLEANUP_FILES="$CLEANUP_FILES $REL_FILE"
            fi
            if [ -f "$REL_FILE" ]; then
                REL_FILES="$REL_FILES $REL_FILE"
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

# Run SDCC linker
$SDCC $FLAGS $REL_FILES
RESULT=$?

# Cleanup temporary files
for FILE in $CLEANUP_FILES; do
    rm -f "$FILE"
done

exit $RESULT
