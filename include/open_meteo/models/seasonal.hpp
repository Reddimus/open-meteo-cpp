// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file seasonal.hpp
/// @brief Seasonal forecast endpoint response types

#include "open_meteo/models/common.hpp"

#include <nlohmann/json_fwd.hpp>

namespace open_meteo {

/// Seasonal forecast response extends WeatherResponse with additional timeseries blocks
struct SeasonalResponse : WeatherResponse {
	std::optional<TimeseriesData> six_hourly;
	std::optional<TimeseriesData> weekly;
	std::optional<TimeseriesData> monthly;
	std::optional<std::unordered_map<std::string, std::string>> six_hourly_units;
	std::optional<std::unordered_map<std::string, std::string>> weekly_units;
	std::optional<std::unordered_map<std::string, std::string>> monthly_units;
};

void from_json(const nlohmann::json& j, SeasonalResponse& resp);

} // namespace open_meteo
