// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// example_geocoding.cpp — minimal usage of the geocoding endpoint.
//
// Run via `make run-geocoding` (configures + builds Release if needed).
// Hits Open-Meteo's free geocoding API to resolve a place name into
// a list of (lat, lon, country, ...) results. Prints the first few
// matches to stdout.

#include "open_meteo/client.hpp"
#include "open_meteo/error.hpp"
#include "open_meteo/models/geocoding.hpp"
#include "open_meteo/models/params.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
	std::string query = (argc > 1) ? argv[1] : "Paris";

	open_meteo::OpenMeteoClient client{open_meteo::OpenMeteoClient::Config{}};

	open_meteo::GeocodingParams params;
	params.name = query;
	params.count = 5;
	params.language = "en";

	open_meteo::Result<open_meteo::GeocodingResponse> result = client.get_geocoding(params);
	if (!result) {
		std::cerr << "Geocoding failed: " << result.error().message << "\n";
		return EXIT_FAILURE;
	}

	const open_meteo::GeocodingResponse& response = result.value();
	if (response.results.empty()) {
		std::cout << "No results for \"" << query << "\".\n";
		return EXIT_SUCCESS;
	}

	std::cout << "Top " << response.results.size() << " result(s) for \"" << query << "\":\n";
	for (const open_meteo::GeocodingResult& hit : response.results) {
		std::cout << "  - " << hit.name;
		if (hit.admin1)
			std::cout << ", " << *hit.admin1;
		std::cout << " (" << hit.country_code << ")" << "  lat=" << hit.latitude
				  << "  lon=" << hit.longitude << "  tz=" << hit.timezone << "\n";
	}
	return EXIT_SUCCESS;
}
