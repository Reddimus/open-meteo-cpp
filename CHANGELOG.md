# Changelog

All notable changes to **open-meteo-cpp** are recorded here. The format
follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and
the project uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Four new working examples under `examples/` — `forecast`,
  `historical`, `geocoding`, and `air_quality` — each runs against
  the live Open-Meteo endpoints and prints structured output. Gives
  consumers a one-command starting point per use case
  ([`a7e957a`](https://github.com/Reddimus/open-meteo-cpp/commit/a7e957a)).

### Fixed

- Previous-run requests for a single model now serialize the
  `model_name` query parameter — older requests dropped it on the
  single-model path, which the API silently treated as "all models"
  ([`5206446`](https://github.com/Reddimus/open-meteo-cpp/commit/5206446)).
- Examples migrated to explicit local types so `cpp_auto_audit`
  passes ([`3b48a57`](https://github.com/Reddimus/open-meteo-cpp/commit/3b48a57)).
- `cpp_auto_audit.py` now walks `--cached + --others` so new test
  files don't pass local lint and fail CI
  ([`818d4d6`](https://github.com/Reddimus/open-meteo-cpp/commit/818d4d6)).

### CI

- `build-windows` job added via vcpkg (parity with the rest of the
  SDK family).
- `release.yml` body sourced from this CHANGELOG via `--notes-file`
  so inline ` `code` ` spans survive
  ([`03e4f82`](https://github.com/Reddimus/open-meteo-cpp/commit/03e4f82)).
- Tag/CMakeLists `VERSION` drift caught at release time
  ([`eea9a19`](https://github.com/Reddimus/open-meteo-cpp/commit/eea9a19)).
- `actions/checkout@v6` upgrade for Node 24 runtime
  ([`1d9b88d`](https://github.com/Reddimus/open-meteo-cpp/commit/1d9b88d)).
- `MD004` (asterisk style) disabled for prose continuation parity
  ([`8ca94f5`](https://github.com/Reddimus/open-meteo-cpp/commit/8ca94f5)).

### Docs

- Add a Contributing section to the README
  ([`c404547`](https://github.com/Reddimus/open-meteo-cpp/commit/c404547)).
- Add CI / release / C++ standard / license badges to the top of the
  README ([`9e3470e`](https://github.com/Reddimus/open-meteo-cpp/commit/9e3470e)).
- Squash double-blank-line inserted by the Contributing section
  ([`776060f`](https://github.com/Reddimus/open-meteo-cpp/commit/776060f)).
- Add `CLAUDE.md` build/architecture/conventions reference
  ([`bc3a11a`](https://github.com/Reddimus/open-meteo-cpp/commit/bc3a11a)).
- Add this `CHANGELOG.md` + the auto-release-on-tag workflow it feeds
  ([`b364d25`](https://github.com/Reddimus/open-meteo-cpp/commit/b364d25)).

### Build

- Enforce explicit local cpp types
  ([`ec9716a`](https://github.com/Reddimus/open-meteo-cpp/commit/ec9716a)).

### Chore

- Production-hardening `.gitignore` patterns mirrored from the rest
  of the SDK family
  ([`9b941f3`](https://github.com/Reddimus/open-meteo-cpp/commit/9b941f3)).

## [0.1.2] — 2026-04-17

### Fixed

- `EnsembleParams::build_url()` now correctly targets
  `ensemble-api.open-meteo.com` (was using the forecast hostname).
  ([`895aaa8`](https://github.com/Reddimus/open-meteo-cpp/commit/895aaa8))

### Refactor

- Replace `std::ostringstream` with `std::format` throughout
  ([`dbad9ed`](https://github.com/Reddimus/open-meteo-cpp/commit/dbad9ed)).

### Performance

- Default `-march=x86-64-v3` (2017+ baseline: AVX2, BMI2, FMA) when
  `OPEN_METEO_NATIVE_ARCH=OFF`. Matches consumer Dockerfiles for
  identical FetchContent codegen.
  ([`c24c943`](https://github.com/Reddimus/open-meteo-cpp/commit/c24c943))

## [0.1.1]

### Fixed

- `HttpClient::build_url` honors absolute URLs (was double-prefixing
  the base when given a fully-qualified URL).
  ([`50cef06`](https://github.com/Reddimus/open-meteo-cpp/commit/50cef06))

## [0.1.0] — initial public release

### Added

- C++23 SDK for the Open-Meteo Weather API — all 12 endpoints
  (forecast, historical, ensemble, climate, marine, air-quality,
  flood, elevation, geocoding, etc.)
- Layered static libraries: `open_meteo_core` → `open_meteo_http`
  → `open_meteo_models` → `open_meteo_api` → `open_meteo` (INTERFACE)
- `std::expected<T, Error>` for all returns; no exceptions
- Pimpl pattern for `HttpClient` and `OpenMeteoClient`
- Built-in rate limiting + retry with exponential backoff
- Mermaid architecture diagrams + citations in README

[Unreleased]: https://github.com/Reddimus/open-meteo-cpp/compare/v0.1.2...HEAD
[0.1.2]: https://github.com/Reddimus/open-meteo-cpp/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/Reddimus/open-meteo-cpp/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/Reddimus/open-meteo-cpp/releases/tag/v0.1.0
