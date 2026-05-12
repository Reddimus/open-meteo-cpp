// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file elevation.hpp
/// @brief Elevation endpoint response types

#include "open_meteo/error.hpp"

#include <string_view>
#include <vector>

namespace open_meteo {

/// Elevation API response
struct ElevationResponse {
	std::vector<double> elevation;
};

[[nodiscard]] Result<void> deserialize_elevation_response(std::string_view body,
														  ElevationResponse& out);

} // namespace open_meteo
