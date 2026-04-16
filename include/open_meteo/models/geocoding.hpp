// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file geocoding.hpp
/// @brief Geocoding endpoint response types

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <vector>

namespace open_meteo {

/// A single geocoding search result
struct GeocodingResult {
	int id{0};
	std::string name;
	double latitude{0.0};
	double longitude{0.0};
	double elevation{0.0};
	std::string timezone;
	std::string country;
	std::string country_code;
	std::optional<std::string> admin1;
	std::optional<std::string> admin2;
	std::optional<std::string> admin3;
	std::optional<int> population;
	std::optional<std::vector<std::string>> postcodes;
};

/// Geocoding search response
struct GeocodingResponse {
	std::vector<GeocodingResult> results;
	double generationtime_ms{0.0};
};

void from_json(const nlohmann::json& j, GeocodingResult& result);
void from_json(const nlohmann::json& j, GeocodingResponse& resp);

} // namespace open_meteo
