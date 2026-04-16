// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/params.hpp"

#include <sstream>

namespace open_meteo {

// ===== Query string builder helpers =====

namespace {

void ensure_separator(std::string& query) {
	if (query.empty()) {
		query += '?';
	} else {
		query += '&';
	}
}

} // namespace

void append_param(std::string& query, const std::string& key, const std::string& value) {
	ensure_separator(query);
	query += key;
	query += '=';
	query += value;
}

void append_list_param(std::string& query, const std::string& key,
					   const std::vector<std::string>& values) {
	if (values.empty()) {
		return;
	}
	ensure_separator(query);
	query += key;
	query += '=';
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (i > 0) {
			query += ',';
		}
		query += values[i];
	}
}

void append_param(std::string& query, const std::string& key, double value) {
	std::ostringstream oss;
	oss << value;
	append_param(query, key, oss.str());
}

void append_param(std::string& query, const std::string& key, int value) {
	append_param(query, key, std::to_string(value));
}

void append_param(std::string& query, const std::string& key, bool value) {
	append_param(query, key, std::string(value ? "true" : "false"));
}

// ===== Helper to append common forecast-like fields =====

namespace {

void append_common_weather_params(std::string& query,
								  const std::optional<std::vector<std::string>>& hourly,
								  const std::optional<std::vector<std::string>>& daily,
								  const std::optional<std::vector<std::string>>& current,
								  const std::optional<int>& forecast_days,
								  const std::optional<int>& past_days,
								  const std::optional<std::vector<std::string>>& models,
								  const std::optional<std::string>& temperature_unit,
								  const std::optional<std::string>& wind_speed_unit,
								  const std::optional<std::string>& precipitation_unit,
								  const std::optional<std::string>& timezone,
								  const std::optional<std::string>& cell_selection,
								  const std::optional<std::string>& start_date,
								  const std::optional<std::string>& end_date) {
	if (hourly) {
		append_list_param(query, "hourly", *hourly);
	}
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (current) {
		append_list_param(query, "current", *current);
	}
	if (forecast_days) {
		append_param(query, "forecast_days", *forecast_days);
	}
	if (past_days) {
		append_param(query, "past_days", *past_days);
	}
	if (models) {
		append_list_param(query, "models", *models);
	}
	if (temperature_unit) {
		append_param(query, "temperature_unit", *temperature_unit);
	}
	if (wind_speed_unit) {
		append_param(query, "wind_speed_unit", *wind_speed_unit);
	}
	if (precipitation_unit) {
		append_param(query, "precipitation_unit", *precipitation_unit);
	}
	if (timezone) {
		append_param(query, "timezone", *timezone);
	}
	if (cell_selection) {
		append_param(query, "cell_selection", *cell_selection);
	}
	if (start_date) {
		append_param(query, "start_date", *start_date);
	}
	if (end_date) {
		append_param(query, "end_date", *end_date);
	}
}

} // namespace

// ===== ForecastParams =====

std::string ForecastParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	append_common_weather_params(query, hourly, daily, current, forecast_days, past_days, models,
								temperature_unit, wind_speed_unit, precipitation_unit, timezone,
								cell_selection, start_date, end_date);
	return query;
}

// ===== HistoricalParams =====

std::string HistoricalParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	append_param(query, "start_date", start_date);
	append_param(query, "end_date", end_date);
	if (hourly) {
		append_list_param(query, "hourly", *hourly);
	}
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (models) {
		append_list_param(query, "models", *models);
	}
	if (temperature_unit) {
		append_param(query, "temperature_unit", *temperature_unit);
	}
	if (wind_speed_unit) {
		append_param(query, "wind_speed_unit", *wind_speed_unit);
	}
	if (precipitation_unit) {
		append_param(query, "precipitation_unit", *precipitation_unit);
	}
	if (timezone) {
		append_param(query, "timezone", *timezone);
	}
	if (cell_selection) {
		append_param(query, "cell_selection", *cell_selection);
	}
	return query;
}

// ===== HistoricalForecastParams =====

std::string HistoricalForecastParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	append_common_weather_params(query, hourly, daily, current, forecast_days, past_days, models,
								temperature_unit, wind_speed_unit, precipitation_unit, timezone,
								cell_selection, start_date, end_date);
	return query;
}

// ===== PreviousRunsParams =====

std::string PreviousRunsParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	append_common_weather_params(query, hourly, daily, current, forecast_days, past_days, models,
								temperature_unit, wind_speed_unit, precipitation_unit, timezone,
								cell_selection, start_date, end_date);
	if (model_name) {
		append_param(query, "model_name", *model_name);
	}
	if (previous_day) {
		append_param(query, "previous_day", *previous_day);
	}
	return query;
}

// ===== EnsembleParams =====

std::string EnsembleParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	append_common_weather_params(query, hourly, daily, current, forecast_days, past_days, models,
								temperature_unit, wind_speed_unit, precipitation_unit, timezone,
								cell_selection, start_date, end_date);
	return query;
}

// ===== MarineParams =====

std::string MarineParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	if (hourly) {
		append_list_param(query, "hourly", *hourly);
	}
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (current) {
		append_list_param(query, "current", *current);
	}
	if (start_date) {
		append_param(query, "start_date", *start_date);
	}
	if (end_date) {
		append_param(query, "end_date", *end_date);
	}
	if (timezone) {
		append_param(query, "timezone", *timezone);
	}
	if (forecast_days) {
		append_param(query, "forecast_days", *forecast_days);
	}
	if (past_days) {
		append_param(query, "past_days", *past_days);
	}
	if (cell_selection) {
		append_param(query, "cell_selection", *cell_selection);
	}
	if (length_unit) {
		append_param(query, "length_unit", *length_unit);
	}
	return query;
}

// ===== AirQualityParams =====

std::string AirQualityParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	if (hourly) {
		append_list_param(query, "hourly", *hourly);
	}
	if (current) {
		append_list_param(query, "current", *current);
	}
	if (start_date) {
		append_param(query, "start_date", *start_date);
	}
	if (end_date) {
		append_param(query, "end_date", *end_date);
	}
	if (timezone) {
		append_param(query, "timezone", *timezone);
	}
	if (forecast_days) {
		append_param(query, "forecast_days", *forecast_days);
	}
	if (past_days) {
		append_param(query, "past_days", *past_days);
	}
	if (cell_selection) {
		append_param(query, "cell_selection", *cell_selection);
	}
	return query;
}

// ===== ClimateParams =====

std::string ClimateParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (start_date) {
		append_param(query, "start_date", *start_date);
	}
	if (end_date) {
		append_param(query, "end_date", *end_date);
	}
	if (models) {
		append_list_param(query, "models", *models);
	}
	if (temperature_unit) {
		append_param(query, "temperature_unit", *temperature_unit);
	}
	if (wind_speed_unit) {
		append_param(query, "wind_speed_unit", *wind_speed_unit);
	}
	if (precipitation_unit) {
		append_param(query, "precipitation_unit", *precipitation_unit);
	}
	return query;
}

// ===== SeasonalParams =====

std::string SeasonalParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	if (six_hourly) {
		append_list_param(query, "six_hourly", *six_hourly);
	}
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (weekly) {
		append_list_param(query, "weekly", *weekly);
	}
	if (monthly) {
		append_list_param(query, "monthly", *monthly);
	}
	if (models) {
		append_list_param(query, "models", *models);
	}
	if (temperature_unit) {
		append_param(query, "temperature_unit", *temperature_unit);
	}
	if (wind_speed_unit) {
		append_param(query, "wind_speed_unit", *wind_speed_unit);
	}
	if (precipitation_unit) {
		append_param(query, "precipitation_unit", *precipitation_unit);
	}
	if (timezone) {
		append_param(query, "timezone", *timezone);
	}
	return query;
}

// ===== FloodParams =====

std::string FloodParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	if (daily) {
		append_list_param(query, "daily", *daily);
	}
	if (start_date) {
		append_param(query, "start_date", *start_date);
	}
	if (end_date) {
		append_param(query, "end_date", *end_date);
	}
	if (ensemble) {
		append_param(query, "ensemble", *ensemble);
	}
	if (forecast_days) {
		append_param(query, "forecast_days", *forecast_days);
	}
	if (past_days) {
		append_param(query, "past_days", *past_days);
	}
	return query;
}

// ===== ElevationParams =====

std::string ElevationParams::build_query_string() const {
	std::string query;
	append_param(query, "latitude", latitude);
	append_param(query, "longitude", longitude);
	return query;
}

// ===== GeocodingParams =====

std::string GeocodingParams::build_query_string() const {
	std::string query;
	append_param(query, "name", name);
	if (count) {
		append_param(query, "count", *count);
	}
	if (language) {
		append_param(query, "language", *language);
	}
	if (format) {
		append_param(query, "format", *format);
	}
	return query;
}

} // namespace open_meteo
