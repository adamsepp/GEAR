# -----------------------------------------------------------------------------
# EmbedFileAsHeader.cmake
#
# Purpose:
#   Reads a binary file (e.g. PNG icon) and generates a C/C++ header file
#   containing the file's bytes as a static unsigned char array, plus a length
#   variable. This allows embedding assets (icons, shaders, etc.) directly
#   into the compiled binary without shipping external files.
#
# Usage:
#   cmake -DINPUT=<path to source file>
#         -DOUTPUT=<path to generated header>
#         -DSYMBOL=<name for the C++ array>
#         -P EmbedFileAsHeader.cmake
#
# Example:
#   cmake -DINPUT=Assets/Icon16x16.png
#         -DOUTPUT=build/Assets/IconsGenerated/Icon16x16.h
#         -DSYMBOL=Icon16x16
#         -P Assets/EmbedFileAsHeader.cmake
#
# Result:
#   Generates a header file like:
#       static const unsigned char Icon16x16[] = { 0x89, 0x50, 0x4E, ... };
#       static const unsigned int  Icon16x16_len = 1234;
#
# Notes:
#   - No external tools like `xxd` are required.
#   - Works on all platforms where CMake runs.
#   - The SYMBOL should be a valid C identifier (no spaces, dots, etc.).
# -----------------------------------------------------------------------------

if(NOT DEFINED INPUT OR NOT DEFINED OUTPUT OR NOT DEFINED SYMBOL)
  message(FATAL_ERROR "Need -DINPUT=, -DOUTPUT=, -DSYMBOL=")
endif()

# Read binary as hex (two chars per byte, no separators)
file(READ "${INPUT}" HEX_CONTENT HEX)
string(LENGTH "${HEX_CONTENT}" HEX_LEN)

# Build comma-separated 0x.. list with line breaks
set(LINE "")
set(COUNT 0)
set(BYTES "")
math(EXPR N_BYTES "${HEX_LEN} / 2")
set(INDEX 0)
while(INDEX LESS HEX_LEN)
  string(SUBSTRING "${HEX_CONTENT}" ${INDEX} 2 BYTEHEX)
  string(APPEND LINE "0x${BYTEHEX}, ")
  math(EXPR COUNT "${COUNT}+1")
  if(COUNT EQUAL 12)
    string(APPEND BYTES "  ${LINE}\n")
    set(LINE "")
    set(COUNT 0)
  endif()
  math(EXPR INDEX "${INDEX}+2")
endwhile()
if(NOT LINE STREQUAL "")
  string(APPEND BYTES "  ${LINE}\n")
endif()

set(OUT_TEXT "/* Generated from ${INPUT} */\n#pragma once\n")
string(APPEND OUT_TEXT "static const unsigned char ${SYMBOL}[] = {\n${BYTES}};\n")
string(APPEND OUT_TEXT "static const unsigned int ${SYMBOL}_len = ${N_BYTES};\n")

file(WRITE "${OUTPUT}" "${OUT_TEXT}")
message(STATUS "Embedded ${INPUT} -> ${OUTPUT} as ${SYMBOL} (${N_BYTES} bytes)")
