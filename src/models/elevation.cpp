// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/elevation.hpp"

#include <nlohmann/json.hpp>

namespace open_meteo {

void from_json(const nlohmann::json& j, ElevationResponse& resp) {
	if (j.contains("elevation") && j["elevation"].is_array()) {
		resp.elevation.reserve(j["elevation"].size());
		for (const nlohmann::json& elem : j["elevation"]) {
			if (elem.is_number()) {
				resp.elevation.push_back(elem.get<double>());
			} else {
				resp.elevation.push_back(0.0);
			}
		}
	}
}

} // namespace open_meteo
