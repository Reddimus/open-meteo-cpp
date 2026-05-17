// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// example_ensemble.cpp — multi-model ensemble usage.
//
// Run via `make run-ensemble`. Requests per-member 2 m-temperature from
// several ensemble systems for New York and prints the first few member
// series. This is the request shape open-meteo-data's native per-member
// backend relies on (multi-model, >=16-day horizon, per-member daily).

#include "open_meteo/client.hpp"
#include "open_meteo/error.hpp"
#include "open_meteo/models/ensemble.hpp"
#include "open_meteo/models/params.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	double latitude = (argc > 1) ? std::stod(argv[1]) : 40.7128;
	double longitude = (argc > 2) ? std::stod(argv[2]) : -74.0060;

	open_meteo::OpenMeteoClient client{open_meteo::OpenMeteoClient::Config{}};

	open_meteo::EnsembleParams params;
	params.latitude = latitude;
	params.longitude = longitude;
	params.hourly = std::vector<std::string>{"temperature_2m"};
	params.models = std::vector<std::string>{"gfs_seamless", "ecmwf_ifs025"};
	params.forecast_days = 16;

	open_meteo::Result<open_meteo::EnsembleResponse> result = client.get_ensemble(params);
	if (!result) {
		std::cerr << "Ensemble failed: " << result.error().message << "\n";
		return EXIT_FAILURE;
	}

	const open_meteo::EnsembleResponse& ens = result.value();
	std::cout << "Location: " << ens.latitude << ", " << ens.longitude << "\n";
	if (!ens.hourly || ens.hourly->time.empty()) {
		std::cout << "(no hourly data)\n";
		return EXIT_SUCCESS;
	}

	std::size_t shown = 0;
	for (const std::pair<const std::string, std::vector<double>>& kv : ens.hourly->values) {
		if (kv.first.rfind("temperature_2m", 0) != 0 || kv.second.empty()) {
			continue;
		}
		std::cout << "  " << kv.first << "  t0=" << kv.second.front() << "°C\n";
		if (++shown >= 8) {
			break;
		}
	}
	std::cout << "(" << shown << " member series shown)\n";
	return EXIT_SUCCESS;
}
