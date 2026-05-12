// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file seasonal.hpp
/// @brief Seasonal forecast endpoint response types

#include "open_meteo/error.hpp"
#include "open_meteo/models/common.hpp"

#include <string_view>

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

[[nodiscard]] Result<void> deserialize_seasonal_response(std::string_view body,
														 SeasonalResponse& out);

} // namespace open_meteo
