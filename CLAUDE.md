# Development

Run `make build`, `make test`, and `make lint`. Use `make format` before
committing. `make debug` uses build-debug; sanitizers require
`-DOPEN_METEO_ENABLE_SANITIZERS=ON`. `make consumers` checks installation
and FetchContent; `python3 tools/test_install_hooks.py` protects custom hooks.

C++23, namespace open_meteo, tabs, 100 columns. Prefer explicit local types;
iterators, structured bindings, and lambda closures are exceptions.

Layers: core (errors/retry/rate limiting), HTTP (libcurl), models (private
Glaze 9), API client. The umbrella target is open_meteo::open_meteo.

Use src/json.hpp options for bounded complete JSON reads. Preserve dynamic
variable keys and null-to-NaN array positions. Current scalar values become
one-element arrays. Every retry must acquire a token. HTTP clients and API
clients need external synchronization; the token bucket is thread-safe.

Coverage is 12 core API families, not the whole upstream product. See README
for compiler/dependency floors and examples. CI builds and tests Linux,
macOS, and Windows. Releases follow the version in CMakeLists.txt.
