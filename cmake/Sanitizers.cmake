# ==========================================
# Sanitizers.cmake
# ==========================================
#
# Optional sanitizer instrumentation for ChaosNexus targets.
#
# Controlled by the NEXUS_ENABLE_SANITIZERS option (default OFF).
# MSVC only supports AddressSanitizer (no UBSan/TSan as of current
# toolchains), while Clang supports the full sanitizer suite. This
# function branches accordingly rather than assuming Clang-only
# behavior, since the target environment is MSVC-primary.

function(nexus_enable_sanitizers target)
    if(NOT NEXUS_ENABLE_SANITIZERS)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /fsanitize=address)
        target_link_options(${target} PRIVATE /fsanitize=address)
        message(STATUS "nexus_enable_sanitizers: AddressSanitizer enabled for ${target} (MSVC)")

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )
        message(STATUS "nexus_enable_sanitizers: ASan+UBSan enabled for ${target} (Clang/GNU)")
    endif()
endfunction()