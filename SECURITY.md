# Security Policy

`open-meteo-cpp` is a third-party C++ client for the Open-Meteo
weather APIs. The free tier is unauthenticated; customer-tier requests
forward a key set on the client. A vulnerability in credential handling
or in request construction could leak the customer key or smuggle a
forged request. This file is the canonical contact path for reporting
one.

## Supported Versions

Security fixes are made on the latest published `vX.Y.Z` tag. Older
tags are not back-patched — bump your `FetchContent_Declare(... GIT_TAG ...)`
pin or your `find_package(open-meteo-cpp X.Y.Z REQUIRED)` constraint to
the latest minor on the same major as part of the upgrade.

| Version    | Supported          |
| ---------- | ------------------ |
| latest tag | :white_check_mark: |
| older      | :x:                |

## Reporting a Vulnerability

**Do not open a public issue.** Use GitHub's [private vulnerability
reporting](https://github.com/Reddimus/open-meteo-cpp/security/advisories/new)
flow, which delivers the report to the maintainer privately and tracks
coordinated disclosure.

When reporting, please include:

- Affected version (tag or commit SHA)
- A reproduction — minimal code or test case
- Impact (credential leak / forged request / DoS / something else)
- Whether you've notified anyone else (e.g. Open-Meteo directly)

You can expect:

- Acknowledgement within **3 business days**
- An initial assessment + severity rating within **7 business days**
- A fix on a new `vX.Y.Z+1` tag, or a clear timeline if the fix is
  larger

## Out of Scope

- Bugs against `open-meteo.com` itself — those go to Open-Meteo's own
  channel, not this client library.
- Operational issues (rate-limit handling, network blips) — file a
  regular issue.
- Theoretical issues against dependencies — report them upstream
  (`openssl`, `libcurl`, `nlohmann/json`, `googletest`). We pin via
  FetchContent and bump on credible advisories.
