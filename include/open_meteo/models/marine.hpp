// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file marine.hpp
/// @brief Marine weather endpoint response types

#include "open_meteo/models/common.hpp"

namespace open_meteo {

/// Marine weather response (same JSON structure, different variable names)
using MarineResponse = WeatherResponse;

} // namespace open_meteo
