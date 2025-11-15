#!/bin/bash
# SDAR Wrapper Script for Arduino
# Handles conversion between Arduino's .a archives and SDCC's .lib archives

SDAR="$1"
ARCHIVE="$2"
OBJECT="$3"
MODE="$4"
shift 4
FLAGS="$@"

# Convert .a to .lib for SDCC
ARCHIVE_LIB="${ARCHIVE%.a}.lib"
OBJECT_REL="${OBJECT%.o}.rel"

# Convert .o to .rel temporarily
if [ -f "$OBJECT" ]; then
    cp "$OBJECT" "$OBJECT_REL"
fi

# Run sdar with .lib extension
$SDAR $FLAGS "$ARCHIVE_LIB" "$OBJECT_REL"

# Convert .lib back to .a for Arduino
if [ -f "$ARCHIVE_LIB" ]; then
    cp "$ARCHIVE_LIB" "$ARCHIVE"
fi

# Cleanup temporary .rel file
rm -f "$OBJECT_REL"

exit 0
