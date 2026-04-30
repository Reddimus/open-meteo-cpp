// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// example_air_quality.cpp — minimal usage of the air-quality endpoint.
// Run via `make run-air-quality`.

#include "open_meteo/client.hpp"
#include "open_meteo/error.hpp"
#include "open_meteo/models/air_quality.hpp"
#include "open_meteo/models/params.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	double latitude = (argc > 1) ? std::stod(argv[1]) : 40.7128;
	double longitude = (argc > 2) ? std::stod(argv[2]) : -74.0060;

	open_meteo::OpenMeteoClient client{open_meteo::OpenMeteoClient::Config{}};

	open_meteo::AirQualityParams params;
	params.latitude = latitude;
	params.longitude = longitude;
	params.hourly = std::vector<std::string>{"pm10", "pm2_5", "us_aqi"};
	params.forecast_days = 1;

	open_meteo::Result<open_meteo::AirQualityResponse> result = client.get_air_quality(params);
	if (!result) {
		std::cerr << "Air-quality query failed: " << result.error().message << "\n";
		return EXIT_FAILURE;
	}

	const open_meteo::AirQualityResponse& aq = result.value();
	std::cout << "Air quality for (" << aq.latitude << ", " << aq.longitude << "):\n";

	if (!aq.hourly || aq.hourly->time.empty()) {
		std::cout << "(no hourly data)\n";
		return EXIT_SUCCESS;
	}

	auto pm25_it = aq.hourly->values.find("pm2_5");
	auto pm10_it = aq.hourly->values.find("pm10");
	auto aqi_it = aq.hourly->values.find("us_aqi");
	const std::vector<std::string>& times = aq.hourly->time;
	std::size_t n = std::min<std::size_t>(6, times.size());
	std::cout << "First " << n << " hourly samples:\n";
	for (std::size_t i = 0; i < n; ++i) {
		std::cout << "  " << times[i];
		if (pm25_it != aq.hourly->values.end() && i < pm25_it->second.size())
			std::cout << "  pm2_5=" << pm25_it->second[i];
		if (pm10_it != aq.hourly->values.end() && i < pm10_it->second.size())
			std::cout << "  pm10=" << pm10_it->second[i];
		if (aqi_it != aq.hourly->values.end() && i < aqi_it->second.size())
			std::cout << "  us_aqi=" << aqi_it->second[i];
		std::cout << "\n";
	}
	return EXIT_SUCCESS;
}
