// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file params.hpp
/// @brief Request parameter structs for all 12 Open-Meteo API endpoints
///
/// Each struct contains the query parameters accepted by its endpoint.
/// Call build_query_string() to serialize into a URL query string.

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace open_meteo {

// ===== Query string builder helpers =====

/// URL-encode a single parameter and append to query string
void append_param(std::string& query, const std::string& key, const std::string& value);

/// Append a comma-separated list parameter
void append_list_param(std::string& query, const std::string& key,
					   const std::vector<std::string>& values);

/// Append a double parameter
void append_param(std::string& query, const std::string& key, double value);

/// Append an int parameter
void append_param(std::string& query, const std::string& key, int value);

/// Append a bool parameter (true/false)
void append_param(std::string& query, const std::string& key, bool value);

// ===== Forecast =====

/// Parameters for the Forecast API (api.open-meteo.com/v1/forecast)
struct ForecastParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> current;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;
	std::optional<std::string> cell_selection;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Historical =====

/// Parameters for the Historical Weather API (archive-api.open-meteo.com/v1/archive)
struct HistoricalParams {
	double latitude{0.0};
	double longitude{0.0};
	std::string start_date;	 // required
	std::string end_date;	 // required
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;
	std::optional<std::string> cell_selection;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Historical Forecast =====

/// Parameters for the Historical Forecast API
/// (historical-forecast-api.open-meteo.com/v1/forecast)
struct HistoricalForecastParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> current;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;
	std::optional<std::string> cell_selection;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Previous Runs =====

/// Parameters for the Previous Runs API (previous-runs-api.open-meteo.com/v1/forecast)
struct PreviousRunsParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> current;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;
	std::optional<std::string> cell_selection;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;
	std::optional<std::string> model_name;
	std::optional<int> previous_day;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Ensemble =====

/// Parameters for the Ensemble API (api.open-meteo.com/v1/ensemble)
struct EnsembleParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> current;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::vector<std::string>> models; // ensemble-specific model names
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;
	std::optional<std::string> cell_selection;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Marine =====

/// Parameters for the Marine Weather API (api.open-meteo.com/v1/marine)
struct MarineParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> current;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;
	std::optional<std::string> timezone;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::string> cell_selection;
	std::optional<std::string> length_unit;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Air Quality =====

/// Parameters for the Air Quality API (air-quality-api.open-meteo.com/v1/air-quality)
struct AirQualityParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> hourly;
	std::optional<std::vector<std::string>> current;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;
	std::optional<std::string> timezone;
	std::optional<int> forecast_days;
	std::optional<int> past_days;
	std::optional<std::string> cell_selection;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Climate =====

/// Parameters for the Climate Change API (climate-api.open-meteo.com/v1/climate)
struct ClimateParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> daily;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Seasonal =====

/// Parameters for the Seasonal Forecast API (seasonal-api.open-meteo.com/v1/seasonal)
struct SeasonalParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> six_hourly;
	std::optional<std::vector<std::string>> daily;
	std::optional<std::vector<std::string>> weekly;
	std::optional<std::vector<std::string>> monthly;
	std::optional<std::vector<std::string>> models;
	std::optional<std::string> temperature_unit;
	std::optional<std::string> wind_speed_unit;
	std::optional<std::string> precipitation_unit;
	std::optional<std::string> timezone;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Flood =====

/// Parameters for the Flood API (flood-api.open-meteo.com/v1/flood)
struct FloodParams {
	double latitude{0.0};
	double longitude{0.0};
	std::optional<std::vector<std::string>> daily;
	std::optional<std::string> start_date;
	std::optional<std::string> end_date;
	std::optional<bool> ensemble;
	std::optional<int> forecast_days;
	std::optional<int> past_days;

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Elevation =====

/// Parameters for the Elevation API (api.open-meteo.com/v1/elevation)
struct ElevationParams {
	double latitude{0.0};
	double longitude{0.0};

	[[nodiscard]] std::string build_query_string() const;
};

// ===== Geocoding =====

/// Parameters for the Geocoding API (geocoding-api.open-meteo.com/v1/search)
struct GeocodingParams {
	std::string name;
	std::optional<int> count;
	std::optional<std::string> language;
	std::optional<std::string> format;

	[[nodiscard]] std::string build_query_string() const;
};

} // namespace open_meteo
