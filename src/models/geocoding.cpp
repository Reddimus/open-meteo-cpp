// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/geocoding.hpp"

#include "open_meteo/models/common.hpp"

#include <nlohmann/json.hpp>

namespace open_meteo {

void from_json(const nlohmann::json& j, GeocodingResult& result) {
	result.id = json_int(j, "id");
	result.name = json_string(j, "name");
	result.latitude = json_double(j, "latitude");
	result.longitude = json_double(j, "longitude");
	result.elevation = json_double(j, "elevation");
	result.timezone = json_string(j, "timezone");
	result.country = json_string(j, "country");
	result.country_code = json_string(j, "country_code");

	if (j.contains("admin1") && j["admin1"].is_string()) {
		result.admin1 = j["admin1"].get<std::string>();
	}
	if (j.contains("admin2") && j["admin2"].is_string()) {
		result.admin2 = j["admin2"].get<std::string>();
	}
	if (j.contains("admin3") && j["admin3"].is_string()) {
		result.admin3 = j["admin3"].get<std::string>();
	}
	if (j.contains("population") && j["population"].is_number()) {
		result.population = j["population"].get<int>();
	}
	if (j.contains("postcodes") && j["postcodes"].is_array()) {
		std::vector<std::string> codes;
		codes.reserve(j["postcodes"].size());
		for (const nlohmann::json& pc : j["postcodes"]) {
			if (pc.is_string()) {
				codes.push_back(pc.get<std::string>());
			}
		}
		result.postcodes = std::move(codes);
	}
}

void from_json(const nlohmann::json& j, GeocodingResponse& resp) {
	resp.generationtime_ms = json_double(j, "generationtime_ms");

	if (j.contains("results") && j["results"].is_array()) {
		resp.results.reserve(j["results"].size());
		for (const nlohmann::json& item : j["results"]) {
			GeocodingResult result;
			from_json(item, result);
			resp.results.push_back(std::move(result));
		}
	}
}

} // namespace open_meteo
