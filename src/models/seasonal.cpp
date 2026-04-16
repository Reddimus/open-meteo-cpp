// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/seasonal.hpp"

#include "open_meteo/models/common.hpp"

#include <nlohmann/json.hpp>

namespace open_meteo {

namespace {

std::optional<TimeseriesData> parse_timeseries(const nlohmann::json& j, const char* key) {
	if (!j.contains(key) || !j[key].is_object()) {
		return std::nullopt;
	}
	TimeseriesData ts;
	from_json(j[key], ts);
	return ts;
}

std::optional<std::unordered_map<std::string, std::string>>
parse_units_map(const nlohmann::json& j, const char* key) {
	if (!j.contains(key) || !j[key].is_object()) {
		return std::nullopt;
	}
	std::unordered_map<std::string, std::string> units;
	for (const auto& [k, v] : j[key].items()) {
		if (v.is_string()) {
			units[k] = v.get<std::string>();
		}
	}
	return units;
}

} // namespace

void from_json(const nlohmann::json& j, SeasonalResponse& resp) {
	// Parse base WeatherResponse fields
	from_json(j, static_cast<WeatherResponse&>(resp));

	// Parse seasonal-specific timeseries blocks
	resp.six_hourly = parse_timeseries(j, "six_hourly");
	resp.weekly = parse_timeseries(j, "weekly");
	resp.monthly = parse_timeseries(j, "monthly");

	// Parse seasonal-specific unit maps
	resp.six_hourly_units = parse_units_map(j, "six_hourly_units");
	resp.weekly_units = parse_units_map(j, "weekly_units");
	resp.monthly_units = parse_units_map(j, "monthly_units");
}

} // namespace open_meteo
