# Changelog

All notable changes to **open-meteo-cpp** are recorded here. The format
follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and
the project uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Build

- Enforce explicit local cpp types
  ([`ec9716a`](https://github.com/Reddimus/open-meteo-cpp/commit/ec9716a)).

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
