// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file open_meteo.hpp
/// @brief Umbrella header for the Open-Meteo C++ SDK
///
/// Include this single header to get access to all SDK components.

// Infrastructure
#include "open_meteo/client.hpp"
#include "open_meteo/error.hpp"
#include "open_meteo/http_client.hpp"
#include "open_meteo/rate_limit.hpp"
#include "open_meteo/retry.hpp"
#include "open_meteo/types.hpp"

// Models
#include "open_meteo/models/air_quality.hpp"
#include "open_meteo/models/climate.hpp"
#include "open_meteo/models/common.hpp"
#include "open_meteo/models/elevation.hpp"
#include "open_meteo/models/ensemble.hpp"
#include "open_meteo/models/flood.hpp"
#include "open_meteo/models/forecast.hpp"
#include "open_meteo/models/geocoding.hpp"
#include "open_meteo/models/historical.hpp"
#include "open_meteo/models/marine.hpp"
#include "open_meteo/models/params.hpp"
#include "open_meteo/models/seasonal.hpp"
