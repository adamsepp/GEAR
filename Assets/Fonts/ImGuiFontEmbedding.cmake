# -----------------------------------------------------------------------------
# ImGuiFontEmbedding.cmake
#
# Purpose:
#   Helper to embed TTF/OTF font files directly into the executable as C headers.
#   Generates a .h file with a static unsigned char array containing the font
#   bytes, plus a length variable. This way, no external font files need to be
#   shipped alongside the app.
#
# Platforms:
#   - Works on Windows, Linux, macOS
#   - Uses `xxd -i -n` if available
#
# Usage in CMakeLists.txt:
#   include(${CMAKE_SOURCE_DIR}/Assets/Fonts/ImGuiFontEmbedding.cmake)
#
#   file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/EmbeddedFonts)
#
#   embed_font(${CMAKE_SOURCE_DIR}/Assets/Fonts/Inter-Regular.ttf INTER_REGULAR)
#   embed_font(${CMAKE_SOURCE_DIR}/Assets/Fonts/fa-solid-900.otf  FA_SOLID)
#
#   target_sources(GearLib PRIVATE ${INTER_REGULAR_HDR} ${FA_SOLID_HDR})
#
# Example (runtime C++):
#   #include "EmbeddedFonts/Inter-Regular.h"
#   io.Fonts->AddFontFromMemoryTTF((void*)Inter_Regular_ttf,
#       Inter_Regular_ttf_len, 16.0f, &cfg, ranges);
#
# Notes:
#   - Each call to embed_font(<file> <var>) generates a header file.
#   - Two variables are exported:
#       * <var>_HDR    → path to the generated header
#       * <var>_TARGET → custom target that builds this header
# -----------------------------------------------------------------------------

function(embed_font FONT_FILE VAR_PREFIX)
    # Ensure file exists
    if(NOT EXISTS ${FONT_FILE})
        message(FATAL_ERROR "embed_font(): Font file not found: ${FONT_FILE}")
    endif()

    # Extract plain stem (e.g. "Font Awesome 7 Free-Solid-900")
    get_filename_component(FONT_STEM ${FONT_FILE} NAME_WE)

    # Normalize: replace spaces and dashes with underscores
    string(REGEX REPLACE "[ -]" "_" FONT_STEM_NORM "${FONT_STEM}")

    # Lowercase to ensure valid C identifiers
    string(TOLOWER "${FONT_STEM_NORM}" FONT_SYM)

    # Add extension suffix for clarity
    get_filename_component(FONT_EXT ${FONT_FILE} EXT) # .ttf/.otf
    string(REGEX REPLACE "^\\." "_" FONT_EXT "${FONT_EXT}") # "_ttf" / "_otf"
    set(SYMBOL_NAME "${FONT_SYM}${FONT_EXT}")

    # Path to generated header
    set(HDR_FILE ${CMAKE_BINARY_DIR}/EmbeddedFonts/${FONT_STEM_NORM}.h)

    # Generate header with xxd
    add_custom_command(
        OUTPUT ${HDR_FILE}
        COMMAND xxd -i -n ${SYMBOL_NAME} ${FONT_FILE} > ${HDR_FILE}
        DEPENDS ${FONT_FILE}
        COMMENT "Embedding font ${FONT_FILE} -> ${HDR_FILE} as symbol ${SYMBOL_NAME}"
        VERBATIM
    )

    add_custom_target(${VAR_PREFIX}_font DEPENDS ${HDR_FILE})

    set_source_files_properties(${HDR_FILE}
        PROPERTIES GENERATED TRUE
    )

    # Export header path and target
    set(${VAR_PREFIX}_HDR ${HDR_FILE} PARENT_SCOPE)
    set(${VAR_PREFIX}_TARGET ${VAR_PREFIX}_font PARENT_SCOPE)

    #message(STATUS "embed_font(): FONT_FILE=${FONT_FILE}")
    #message(STATUS "embed_font(): HDR_FILE=${HDR_FILE}")
    #message(STATUS "embed_font(): SYMBOL_NAME=${SYMBOL_NAME}")
endfunction()
