// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// Shared response models and bounded JSON deserializers.

#include "open_meteo/error.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace open_meteo {

/// Metadata about the queried location, present in every weather response
struct Location {
	double latitude{0.0};
	double longitude{0.0};
	double elevation{0.0};
	double generationtime_ms{0.0};
	int utc_offset_seconds{0};
	std::string timezone;
	std::string timezone_abbreviation;
};

/// Time-indexed data; current conditions use one-element arrays.
///
/// Open-Meteo returns a "time" array alongside parallel arrays for each
/// requested variable. Numeric variables go into `values`; string-typed
/// variables (e.g. weather codes as descriptions) go into `string_values`.
struct TimeseriesData {
	std::vector<std::string> time;
	std::unordered_map<std::string, std::vector<double>> values; // variable_name -> array
	std::unordered_map<std::string, std::vector<std::string>> string_values; // for weather codes
};

/// Generic weather API response used by forecast, historical, marine, etc.
///
/// Extends Location with optional timeseries blocks and their unit maps.
struct WeatherResponse : Location {
	std::optional<TimeseriesData> hourly;
	std::optional<TimeseriesData> daily;
	std::optional<TimeseriesData> current;
	std::optional<std::unordered_map<std::string, std::string>> hourly_units;
	std::optional<std::unordered_map<std::string, std::string>> daily_units;
	std::optional<std::unordered_map<std::string, std::string>> current_units;
};

// ===== Deserializers (Glaze-backed, return Result<void>) =====
//
// Each function parses a JSON body (string_view, zero-copy where possible)
// into the corresponding struct. On failure returns Error::parse(...).

[[nodiscard]] Result<void> deserialize_location(std::string_view body, Location& out);

[[nodiscard]] Result<void> deserialize_weather_response(std::string_view body,
														WeatherResponse& out);

} // namespace open_meteo
