// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file common_glaze_detail.hpp
/// @brief Internal Glaze helpers shared across model translation units
///
/// NOT a public API. Lives under `src/models/` (NOT under `include/`) so it
/// is never installed — downstream consumers (crawler, kalshi-trader) cannot
/// accidentally depend on these symbols. They exist only so `seasonal.cpp`
/// etc. can reuse the timeseries-extraction logic without duplicating it.

#include "open_meteo/models/common.hpp"

#include <glaze/glaze.hpp>
#include <glaze/json/generic.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace open_meteo::detail {

/// Populate a Location's scalar fields from a glz::generic root object.
void populate_location(const glz::generic& root, Location& loc);

/// Extract a (possibly absent) timeseries block by key from a generic root.
[[nodiscard]] std::optional<TimeseriesData> grab_timeseries(const glz::generic& root,
															const char* key);

/// Extract a (possibly absent) units map by key from a generic root.
[[nodiscard]] std::optional<std::unordered_map<std::string, std::string>>
grab_units_map(const glz::generic& root, const char* key);

} // namespace open_meteo::detail
