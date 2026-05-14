# open-meteo-cpp Development Guide

## Build & Test

```bash
make build          # Release build (CMake + make)
make debug          # Debug build
make test           # Run unit tests (ctest)
make lint           # Check formatting (clang-format --dry-run) + cpp_auto_audit
make format         # Format in place
make coverage       # lcov coverage report
make clean          # Remove build/

make run-forecast      # examples/example_forecast.cpp
make run-historical    # examples/example_historical.cpp
make run-geocoding     # examples/example_geocoding.cpp
make run-air-quality   # examples/example_air_quality.cpp
```

## Architecture

- **Layered static libraries**: `open_meteo_core` → `open_meteo_http` → `open_meteo_models` → `open_meteo_api` → `open_meteo` (INTERFACE)
- **C++23**: `std::expected<T, Error>` for all returns, no exceptions
- **Patterns**: Pimpl (`HttpClient`, `OpenMeteoClient`), non-copyable/movable clients, `[[nodiscard]]`
- **No-auth API**: Open-Meteo's free tier doesn't require keys; clients can opt into customer-tier subdomains via `Config{.use_customer_endpoints=true}`
- **JSON**: [Glaze](https://github.com/stephenberry/glaze) v7.6.0 via FetchContent (compile-time reflection, ~4x parse speedup over nlohmann on dense numeric payloads — migrated 2026-05-11). See `tests/parse_benchmark.cpp` for the regression guard.
- **Tests**: GoogleTest via FetchContent
- **12 endpoints all implemented**: forecast, historical, historical-forecast, previous-runs, ensemble, marine, air-quality, climate, seasonal, flood, elevation, geocoding — see `include/open_meteo/client.hpp`. Examples ship for 4 (see Makefile).

## Conventions

- Code style: `.clang-format` (LLVM base, tabs, 100 cols)
- Namespace: `open_meteo`
- **No `auto`** for local declarations — use explicit types. Carve-outs: structured bindings (`auto& [k, v]`), lambda closures, iterator-like results from `.find/.begin/.end`. Enforced by `tools/cpp_auto_audit.py` (prints to **stderr**, easy to miss; check exit code or run directly).
- Time-series response shape: `HourlyData{ time: vector<string>, values: unordered_map<string, vector<double>> }` — index `time[i]` corresponds to `values["temperature_2m"][i]` etc.
- Model parsing is exposed via the `deserialize_*(std::string_view, T&) -> Result<void>` family declared in `include/open_meteo/models/*.hpp` and implemented in matching `src/models/*.cpp`. The pre-migration `from_json(const nlohmann::json&, T&)` overloads were removed in the 2026-05-11 Glaze cutover; downstream consumers use `OpenMeteoClient::get_*` directly, never the helpers. Dynamic-key timeseries blocks (user-driven variable names like `temperature_2m`/`precipitation`) walk a `glz::generic` AST — search for `TODO(glaze):` markers.

## CI

GitHub Actions workflow `.github/workflows/ci.yml`: build + test + lint on Ubuntu 24.04, build + lint on macos-latest, markdown-lint via DavidAnson. Release workflow auto-creates a GitHub Release on `vX.Y.Z` tag push (notes extracted from `CHANGELOG.md`).
