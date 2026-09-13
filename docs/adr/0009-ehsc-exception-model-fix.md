# 0009 — Enable /EHsc for MSVC Builds

## Status
Accepted

## Context
Since at least Phase 5 (NEXUS_VERIFY/NexusException), every MSVC build
in this repository has emitted C4530 ("C++ exception handler used, but
unwind semantics are not enabled") on any translation unit that throws
or catches. `cmake/CompilerWarnings.cmake`'s MSVC branch never set
/EHsc. This was tolerated by cl.exe (degraded exception model, builds
and links) but caused clang-tidy's clang-cl driver to hard-error on
every throw/try site, discovered during Phase 11 Step 11O.

## Decision
Add /EHsc to nexus_set_warnings()'s MSVC branch. This is the standard
C++ exception model (synchronous exceptions; extern "C" functions
assumed non-throwing).

## Consequences
- Removes C4530 from all builds project-wide.
- Unblocks clang-tidy analysis of any exception-throwing code.
- No behavioral change to passing tests (290/290 unaffected before
  and after).
- Applies to all current and future targets that call
  nexus_set_warnings(); no per-target opt-out exists or is needed.
