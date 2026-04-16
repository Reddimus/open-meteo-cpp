// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file historical.hpp
/// @brief Historical, historical-forecast, and previous-runs endpoint response types

#include "open_meteo/models/common.hpp"

namespace open_meteo {

/// Historical weather archive response
using HistoricalWeatherResponse = WeatherResponse;

/// Historical forecast model data response
using HistoricalForecastResponse = WeatherResponse;

/// Previous model runs response
using PreviousRunsResponse = WeatherResponse;

} // namespace open_meteo
