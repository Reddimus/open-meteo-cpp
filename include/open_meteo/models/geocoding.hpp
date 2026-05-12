// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file geocoding.hpp
/// @brief Geocoding endpoint response types

#include "open_meteo/error.hpp"

#include <optional>
#include <string>
#include <string_view>
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

[[nodiscard]] Result<void> deserialize_geocoding_response(std::string_view body,
														  GeocodingResponse& out);

} // namespace open_meteo
