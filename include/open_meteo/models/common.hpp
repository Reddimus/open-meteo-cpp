// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file common.hpp
/// @brief Common model types shared across all Open-Meteo API responses

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
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

/// Time-indexed data arrays (hourly, daily, current, etc.)
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

// ===== JSON helpers =====

/// Safely extract a string from JSON (handles null/missing values)
[[nodiscard]] std::string json_string(const nlohmann::json& j, const char* key);

/// Safely extract an int from JSON (handles null/missing values)
[[nodiscard]] int json_int(const nlohmann::json& j, const char* key, int def = 0);

/// Safely extract a double from JSON (handles null/missing values)
[[nodiscard]] double json_double(const nlohmann::json& j, const char* key, double def = 0.0);

// ===== from_json declarations =====

void from_json(const nlohmann::json& j, Location& loc);
void from_json(const nlohmann::json& j, TimeseriesData& ts);
void from_json(const nlohmann::json& j, WeatherResponse& resp);

} // namespace open_meteo
