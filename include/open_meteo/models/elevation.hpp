// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file elevation.hpp
/// @brief Elevation endpoint response types

#include <nlohmann/json_fwd.hpp>
#include <vector>

namespace open_meteo {

/// Elevation API response
struct ElevationResponse {
	std::vector<double> elevation;
};

void from_json(const nlohmann::json& j, ElevationResponse& resp);

} // namespace open_meteo
