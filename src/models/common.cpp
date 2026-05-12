// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/common.hpp"

#include <cmath>
#include <glaze/glaze.hpp>
#include <glaze/json/generic.hpp>
#include <limits>
#include <string>
#include <vector>

#include "common_glaze_detail.hpp"

namespace open_meteo {

// ===== Internal-helper file plan =====
//
// The TimeseriesData payload has DYNAMIC KEYS (variable names come from the
// user's query string), so we cannot pre-declare a glz::meta. We parse a
// full response into `glz::generic` once and walk the resulting AST. The
// inner helpers `populate_*_from_generic`, `extract_*` are exported via
// the `detail::` namespace at the bottom of this file for use by
// `seasonal.cpp`, which extends the WeatherResponse layout with additional
// timeseries blocks (`six_hourly`, `weekly`, `monthly`).
//
// TODO(glaze): once Open-Meteo stabilizes a documented "variables" envelope,
// migrate the dynamic-key dispatch to a static glz::meta to squeeze out the
// remaining ~10% generic-AST overhead.

namespace {

// ===== TimeseriesData parsing via glz::generic =====
//
// Open-Meteo's timeseries blocks look like:
//   { "time": ["2026-05-11T00:00", ...],
//     "temperature_2m": [12.3, 12.4, ...],
//     "weather_code": ["clear", "partly_cloudy", ...] }
// The keys after "time" are user-driven, so we cannot pre-declare a glz::meta.
// We parse the block as a glz::generic and walk it.

void populate_timeseries_from_generic(const glz::generic& node, TimeseriesData& ts) {
	if (!node.is_object()) {
		return;
	}
	const glz::generic::object_t& obj = node.get_object();

	// 1) "time" array (strings)
	glz::generic::object_t::const_iterator time_it = obj.find("time");
	if (time_it != obj.end() && time_it->second.is_array()) {
		const glz::generic::array_t& time_arr = time_it->second.get_array();
		ts.time.reserve(time_arr.size());
		for (const glz::generic& elem : time_arr) {
			if (elem.is_string()) {
				ts.time.push_back(elem.get<std::string>());
			} else {
				ts.time.emplace_back();
			}
		}
	}

	// 2) Other keys = per-variable arrays. Determine numeric vs string by
	//    inspecting the first non-null element.
	for (const auto& [key, val] : obj) {
		if (key == "time") {
			continue;
		}
		if (!val.is_array()) {
			continue;
		}
		const glz::generic::array_t& arr = val.get_array();
		if (arr.empty()) {
			ts.values[key] = {};
			continue;
		}

		bool is_string_array = false;
		for (const glz::generic& elem : arr) {
			if (!elem.is_null()) {
				is_string_array = elem.is_string();
				break;
			}
		}

		if (is_string_array) {
			std::vector<std::string>& str_vec = ts.string_values[key];
			str_vec.reserve(arr.size());
			for (const glz::generic& elem : arr) {
				if (elem.is_string()) {
					str_vec.push_back(elem.get<std::string>());
				} else {
					str_vec.emplace_back();
				}
			}
		} else {
			std::vector<double>& num_vec = ts.values[key];
			num_vec.reserve(arr.size());
			for (const glz::generic& elem : arr) {
				if (elem.is_number()) {
					num_vec.push_back(elem.get<double>());
				} else {
					num_vec.push_back(std::numeric_limits<double>::quiet_NaN());
				}
			}
		}
	}
}

std::optional<TimeseriesData> extract_timeseries(const glz::generic& root, const char* key) {
	if (!root.is_object()) {
		return std::nullopt;
	}
	const glz::generic::object_t& obj = root.get_object();
	glz::generic::object_t::const_iterator it = obj.find(key);
	if (it == obj.end() || !it->second.is_object()) {
		return std::nullopt;
	}
	TimeseriesData ts;
	populate_timeseries_from_generic(it->second, ts);
	return ts;
}

std::optional<std::unordered_map<std::string, std::string>>
extract_units_map(const glz::generic& root, const char* key) {
	if (!root.is_object()) {
		return std::nullopt;
	}
	const glz::generic::object_t& obj = root.get_object();
	glz::generic::object_t::const_iterator it = obj.find(key);
	if (it == obj.end() || !it->second.is_object()) {
		return std::nullopt;
	}
	std::unordered_map<std::string, std::string> units;
	for (const auto& [key, val] : it->second.get_object()) {
		if (val.is_string()) {
			units[key] = val.get<std::string>();
		}
	}
	return units;
}

void populate_location_from_generic(const glz::generic& root, Location& loc) {
	if (!root.is_object()) {
		return;
	}
	const glz::generic::object_t& obj = root.get_object();
	glz::generic::object_t::const_iterator it;

	it = obj.find("latitude");
	if (it != obj.end() && it->second.is_number()) {
		loc.latitude = it->second.get<double>();
	}
	it = obj.find("longitude");
	if (it != obj.end() && it->second.is_number()) {
		loc.longitude = it->second.get<double>();
	}
	it = obj.find("elevation");
	if (it != obj.end() && it->second.is_number()) {
		loc.elevation = it->second.get<double>();
	}
	it = obj.find("generationtime_ms");
	if (it != obj.end() && it->second.is_number()) {
		loc.generationtime_ms = it->second.get<double>();
	}
	it = obj.find("utc_offset_seconds");
	if (it != obj.end() && it->second.is_number()) {
		loc.utc_offset_seconds = static_cast<int>(it->second.get<double>());
	}
	it = obj.find("timezone");
	if (it != obj.end() && it->second.is_string()) {
		loc.timezone = it->second.get<std::string>();
	}
	it = obj.find("timezone_abbreviation");
	if (it != obj.end() && it->second.is_string()) {
		loc.timezone_abbreviation = it->second.get<std::string>();
	}
}

} // namespace

// ===== Public deserializers =====

Result<void> deserialize_location(std::string_view body, Location& out) {
	// Weather payloads have arbitrary sibling keys (timeseries blocks etc.).
	// We parse generic and extract only Location's scalar fields. The
	// non-generic glz::meta path would also work via error_on_unknown_keys
	// = false, but the payload is dominated by timeseries arrays — saving
	// the AST allocation on the scalars doesn't materially help end-to-end
	// throughput, so we prefer the single generic-pass for simplicity.
	glz::generic root{};
	glz::error_ctx ec = glz::read_json(root, body);
	if (ec) {
		return std::unexpected(Error::parse(glz::format_error(ec, body)));
	}
	populate_location_from_generic(root, out);
	return {};
}

Result<void> deserialize_weather_response(std::string_view body, WeatherResponse& out) {
	// Weather payloads have dynamic per-variable keys inside the timeseries
	// blocks. Parse once into glz::generic, then dispatch.
	glz::generic root{};
	glz::error_ctx ec = glz::read_json(root, body);
	if (ec) {
		return std::unexpected(Error::parse(glz::format_error(ec, body)));
	}

	populate_location_from_generic(root, static_cast<Location&>(out));

	out.hourly = extract_timeseries(root, "hourly");
	out.daily = extract_timeseries(root, "daily");
	out.current = extract_timeseries(root, "current");

	out.hourly_units = extract_units_map(root, "hourly_units");
	out.daily_units = extract_units_map(root, "daily_units");
	out.current_units = extract_units_map(root, "current_units");

	return {};
}

// ===== Internal helpers exposed to sibling .cpp files =====

namespace detail {

void populate_location(const glz::generic& root, Location& loc) {
	populate_location_from_generic(root, loc);
}

std::optional<TimeseriesData> grab_timeseries(const glz::generic& root, const char* key) {
	return extract_timeseries(root, key);
}

std::optional<std::unordered_map<std::string, std::string>> grab_units_map(const glz::generic& root,
																		   const char* key) {
	return extract_units_map(root, key);
}

} // namespace detail

} // namespace open_meteo
