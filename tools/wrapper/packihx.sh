#!/bin/bash
# packihx wrapper to handle output redirection

PACKIHX="$1"
INPUT="$2"
OUTPUT="$3"

# Run packihx and redirect output
$PACKIHX "$INPUT" > "$OUTPUT" 2>/dev/null

exit $?
