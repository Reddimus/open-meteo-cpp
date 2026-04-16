// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file forecast.hpp
/// @brief Forecast endpoint response types

#include "open_meteo/models/common.hpp"

namespace open_meteo {

/// Forecast endpoint uses the generic WeatherResponse
using ForecastResponse = WeatherResponse;

} // namespace open_meteo
