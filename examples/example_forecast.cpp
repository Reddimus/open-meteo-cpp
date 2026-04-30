// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// example_forecast.cpp — minimal usage of the forecast endpoint.
//
// Run via `make run-forecast`. Defaults to a 3-day hourly forecast for
// New York; pass `lat lon` as args to query elsewhere.

#include "open_meteo/client.hpp"
#include "open_meteo/error.hpp"
#include "open_meteo/models/forecast.hpp"
#include "open_meteo/models/params.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	double latitude = (argc > 1) ? std::stod(argv[1]) : 40.7128;
	double longitude = (argc > 2) ? std::stod(argv[2]) : -74.0060;

	open_meteo::OpenMeteoClient client{open_meteo::OpenMeteoClient::Config{}};

	open_meteo::ForecastParams params;
	params.latitude = latitude;
	params.longitude = longitude;
	params.hourly = std::vector<std::string>{"temperature_2m", "precipitation"};
	params.forecast_days = 3;

	open_meteo::Result<open_meteo::ForecastResponse> result = client.get_forecast(params);
	if (!result) {
		std::cerr << "Forecast failed: " << result.error().message << "\n";
		return EXIT_FAILURE;
	}

	const open_meteo::ForecastResponse& forecast = result.value();
	std::cout << "Location: " << forecast.latitude << ", " << forecast.longitude
			  << " (tz=" << forecast.timezone << ")\n";

	if (!forecast.hourly) {
		std::cout << "(no hourly data)\n";
		return EXIT_SUCCESS;
	}
	auto temps_it = forecast.hourly->values.find("temperature_2m");
	if (temps_it == forecast.hourly->values.end() || forecast.hourly->time.empty()) {
		std::cout << "(no temperature_2m series)\n";
		return EXIT_SUCCESS;
	}

	const std::vector<std::string>& times = forecast.hourly->time;
	const std::vector<double>& temps = temps_it->second;
	std::size_t n = std::min<std::size_t>(6, std::min(times.size(), temps.size()));
	std::cout << "First " << n << " hourly temperatures:\n";
	for (std::size_t i = 0; i < n; ++i) {
		std::cout << "  " << times[i] << "  " << temps[i] << "°C\n";
	}
	return EXIT_SUCCESS;
}
