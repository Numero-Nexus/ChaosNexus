# ==========================================
# CompilerWarnings.cmake
# ==========================================
#
# Centralized compiler warning policy for ChaosNexus.
#
# Rationale: strict warnings are treated as a first-class engineering
# control, not an afterthought. A single shared function avoids
# warning-flag drift across nexus-core, nexus-sdk, nexus-cli,
# nexus-bench, and nexus-lab.

function(nexus_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4          # High warning level
            /permissive- # Strict standards conformance
            /Zc:__cplusplus
            /w14242      # Possible loss of data (conversions)
            /w14254      # Possible loss of data (bitfields)
            /w14263      # Member function does not override base
            /w14265      # Class has virtual functions but non-virtual destructor
            /w14287      # Unsigned/negative constant mismatch
            /w14296      # Expression always false
            /w14311      # Pointer truncation
            /w14545      # Expression before comma has no effect
            /w14546      # Function call before comma missing argument list
            /w14547      # Operator before comma has no effect
            /w14549      # Operator before comma has no effect
            /w14555      # Expression has no effect
            /w14619      # Unknown pragma warning number
            /w14640      # Thread-unsafe static member initialization
            /w14826      # Sign extension conversion
            /w14905      # Wide string literal cast to LPSTR
            /w14906      # String literal cast to LPWSTR
            /w14928      # Illegal copy-initialization
        )

        if(NEXUS_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )

        if(NEXUS_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
