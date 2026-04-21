// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/common.hpp"

#include <nlohmann/json.hpp>

namespace open_meteo {

std::string json_string(const nlohmann::json& j, const char* key) {
	if (j.contains(key) && j[key].is_string()) {
		return j[key].get<std::string>();
	}
	return {};
}

int json_int(const nlohmann::json& j, const char* key, int def) {
	if (j.contains(key) && j[key].is_number()) {
		return j[key].get<int>();
	}
	return def;
}

double json_double(const nlohmann::json& j, const char* key, double def) {
	if (j.contains(key) && j[key].is_number()) {
		return j[key].get<double>();
	}
	return def;
}

void from_json(const nlohmann::json& j, Location& loc) {
	loc.latitude = json_double(j, "latitude");
	loc.longitude = json_double(j, "longitude");
	loc.elevation = json_double(j, "elevation");
	loc.generationtime_ms = json_double(j, "generationtime_ms");
	loc.utc_offset_seconds = json_int(j, "utc_offset_seconds");
	loc.timezone = json_string(j, "timezone");
	loc.timezone_abbreviation = json_string(j, "timezone_abbreviation");
}

void from_json(const nlohmann::json& j, TimeseriesData& ts) {
	// Parse the "time" array
	if (j.contains("time") && j["time"].is_array()) {
		const nlohmann::json& time_arr = j["time"];
		ts.time.reserve(time_arr.size());
		for (const nlohmann::json& t : time_arr) {
			if (t.is_string()) {
				ts.time.push_back(t.get<std::string>());
			} else {
				ts.time.emplace_back();
			}
		}
	}

	// Parse all other keys as variable arrays
	for (const auto& [key, val] : j.items()) {
		if (key == "time") {
			continue;
		}
		if (!val.is_array()) {
			continue;
		}
		if (val.empty()) {
			ts.values[key] = {};
			continue;
		}

		// Determine if this is a numeric or string array by checking the first non-null element
		bool is_string_array = false;
		for (const nlohmann::json& elem : val) {
			if (!elem.is_null()) {
				is_string_array = elem.is_string();
				break;
			}
		}

		if (is_string_array) {
			std::vector<std::string>& str_vec = ts.string_values[key];
			str_vec.reserve(val.size());
			for (const nlohmann::json& elem : val) {
				if (elem.is_string()) {
					str_vec.push_back(elem.get<std::string>());
				} else {
					str_vec.emplace_back();
				}
			}
		} else {
			std::vector<double>& num_vec = ts.values[key];
			num_vec.reserve(val.size());
			for (const nlohmann::json& elem : val) {
				if (elem.is_number()) {
					num_vec.push_back(elem.get<double>());
				} else {
					// Use NaN for null/missing numeric values
					num_vec.push_back(std::numeric_limits<double>::quiet_NaN());
				}
			}
		}
	}
}

namespace {

/// Parse a units map from JSON (e.g. hourly_units, daily_units)
std::optional<std::unordered_map<std::string, std::string>> parse_units_map(const nlohmann::json& j,
																			const char* key) {
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

/// Parse a timeseries block from JSON (e.g. "hourly", "daily", "current")
std::optional<TimeseriesData> parse_timeseries(const nlohmann::json& j, const char* key) {
	if (!j.contains(key) || !j[key].is_object()) {
		return std::nullopt;
	}
	TimeseriesData ts;
	from_json(j[key], ts);
	return ts;
}

} // namespace

void from_json(const nlohmann::json& j, WeatherResponse& resp) {
	// Parse Location base class fields
	from_json(j, static_cast<Location&>(resp));

	// Parse timeseries blocks
	resp.hourly = parse_timeseries(j, "hourly");
	resp.daily = parse_timeseries(j, "daily");
	resp.current = parse_timeseries(j, "current");

	// Parse unit maps
	resp.hourly_units = parse_units_map(j, "hourly_units");
	resp.daily_units = parse_units_map(j, "daily_units");
	resp.current_units = parse_units_map(j, "current_units");
}

} // namespace open_meteo
