// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// example_historical.cpp — minimal usage of the historical-weather
// endpoint. Run via `make run-historical`. Default queries 1-week of
// daily max temperature for New York ending 30 days ago (Open-Meteo's
// historical archive lags real-time by ~5 days).

#include "open_meteo/client.hpp"
#include "open_meteo/models/historical.hpp"
#include "open_meteo/models/params.hpp"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string format_date(std::chrono::system_clock::time_point tp) {
	std::time_t t = std::chrono::system_clock::to_time_t(tp);
	std::tm tm{};
	gmtime_r(&t, &tm);
	std::ostringstream out;
	out << std::put_time(&tm, "%Y-%m-%d");
	return out.str();
}

} // namespace

int main() {
	auto now = std::chrono::system_clock::now();
	auto end = now - std::chrono::hours(24 * 30);
	auto start = end - std::chrono::hours(24 * 7);

	open_meteo::OpenMeteoClient client{open_meteo::OpenMeteoClient::Config{}};

	open_meteo::HistoricalParams params;
	params.latitude = 40.7128;
	params.longitude = -74.0060;
	params.start_date = format_date(start);
	params.end_date = format_date(end);
	params.daily =
		std::vector<std::string>{"temperature_2m_max", "temperature_2m_min", "precipitation_sum"};

	auto result = client.get_historical(params);
	if (!result) {
		std::cerr << "Historical query failed: " << result.error().message << "\n";
		return EXIT_FAILURE;
	}

	const auto& historical = result.value();
	std::cout << "Daily history for (" << historical.latitude << ", " << historical.longitude
			  << ") from " << params.start_date << " to " << params.end_date << ":\n";

	if (!historical.daily || historical.daily->time.empty()) {
		std::cout << "(no daily data)\n";
		return EXIT_SUCCESS;
	}

	auto max_it = historical.daily->values.find("temperature_2m_max");
	auto min_it = historical.daily->values.find("temperature_2m_min");
	auto pr_it = historical.daily->values.find("precipitation_sum");
	const auto& times = historical.daily->time;
	for (std::size_t i = 0; i < times.size(); ++i) {
		std::cout << "  " << times[i];
		if (max_it != historical.daily->values.end() && i < max_it->second.size())
			std::cout << "  hi=" << max_it->second[i] << "°C";
		if (min_it != historical.daily->values.end() && i < min_it->second.size())
			std::cout << "  lo=" << min_it->second[i] << "°C";
		if (pr_it != historical.daily->values.end() && i < pr_it->second.size())
			std::cout << "  precip=" << pr_it->second[i] << "mm";
		std::cout << "\n";
	}
	return EXIT_SUCCESS;
}
