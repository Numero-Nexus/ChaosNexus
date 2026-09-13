# 0008 — Scheduler Constructor Takes a Raw Policy Name

## Status
Accepted

## Decision
`Scheduler` is constructed with `time::VirtualClock&`, `events::EventQueue&`
(both non-owning), and a `std::string_view policy_name` resolved against
the ADR-0006 catalog. An unresolvable name fails with
`ErrorCode::UnknownSchedulingPolicy` via `Result`, not `NEXUS_VERIFY`
(this is caller/config-input error, not a contract violation).

## Consequences
- Defers Configuration-module (`ConfigValue`) coupling to Phase 12 wiring.
- New `ErrorCode::UnknownSchedulingPolicy` added; no existing code fit.
