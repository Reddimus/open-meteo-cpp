// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// Glaze-deserializer tests — verify the migration's parse output matches
// the documented Open-Meteo response shape and the pre-migration behavior
// (string vs numeric variable detection, NaN-for-null on numeric arrays,
// optional-units handling, etc.).

#include "open_meteo/models/common.hpp"
#include "open_meteo/models/elevation.hpp"
#include "open_meteo/models/forecast.hpp"
#include "open_meteo/models/geocoding.hpp"
#include "open_meteo/models/seasonal.hpp"

#include <cmath>
#include <gtest/gtest.h>
#include <string>

namespace open_meteo {
namespace {

TEST(GlazeDeserializerTest, ParsesForecastWithMixedNumericAndStringSeries) {
	const std::string body = R"({
		"latitude": 40.7128,
		"longitude": -74.006,
		"elevation": 17.0,
		"generationtime_ms": 0.45,
		"utc_offset_seconds": -14400,
		"timezone": "America/New_York",
		"timezone_abbreviation": "EDT",
		"hourly_units": {"temperature_2m": "°C", "weather_code": "wmo"},
		"hourly": {
			"time": ["2026-05-11T00:00", "2026-05-11T01:00", "2026-05-11T02:00"],
			"temperature_2m": [12.3, null, 13.1],
			"weather_code": ["clear", "partly_cloudy", "overcast"]
		}
	})";

	ForecastResponse resp;
	Result<void> r = deserialize_weather_response(body, resp);
	ASSERT_TRUE(r.has_value()) << (r ? "" : r.error().message);

	EXPECT_DOUBLE_EQ(resp.latitude, 40.7128);
	EXPECT_DOUBLE_EQ(resp.longitude, -74.006);
	EXPECT_DOUBLE_EQ(resp.elevation, 17.0);
	EXPECT_EQ(resp.utc_offset_seconds, -14400);
	EXPECT_EQ(resp.timezone, "America/New_York");
	EXPECT_EQ(resp.timezone_abbreviation, "EDT");

	ASSERT_TRUE(resp.hourly.has_value());
	ASSERT_EQ(resp.hourly->time.size(), 3u);
	EXPECT_EQ(resp.hourly->time[0], "2026-05-11T00:00");

	// Numeric series — null becomes NaN
	std::unordered_map<std::string, std::vector<double>>::const_iterator t_it =
		resp.hourly->values.find("temperature_2m");
	ASSERT_NE(t_it, resp.hourly->values.end());
	ASSERT_EQ(t_it->second.size(), 3u);
	EXPECT_DOUBLE_EQ(t_it->second[0], 12.3);
	EXPECT_TRUE(std::isnan(t_it->second[1]));
	EXPECT_DOUBLE_EQ(t_it->second[2], 13.1);

	// String series — landed in string_values, NOT values
	EXPECT_EQ(resp.hourly->values.find("weather_code"), resp.hourly->values.end());
	std::unordered_map<std::string, std::vector<std::string>>::const_iterator w_it =
		resp.hourly->string_values.find("weather_code");
	ASSERT_NE(w_it, resp.hourly->string_values.end());
	ASSERT_EQ(w_it->second.size(), 3u);
	EXPECT_EQ(w_it->second[1], "partly_cloudy");

	ASSERT_TRUE(resp.hourly_units.has_value());
	EXPECT_EQ((*resp.hourly_units)["temperature_2m"], "°C");

	// Daily/current absent => empty optional
	EXPECT_FALSE(resp.daily.has_value());
	EXPECT_FALSE(resp.current.has_value());
}

TEST(GlazeDeserializerTest, ParsesElevationArray) {
	const std::string body = R"({"elevation": [17.0, 23.5, 100.25]})";
	ElevationResponse resp;
	Result<void> r = deserialize_elevation_response(body, resp);
	ASSERT_TRUE(r.has_value()) << (r ? "" : r.error().message);
	ASSERT_EQ(resp.elevation.size(), 3u);
	EXPECT_DOUBLE_EQ(resp.elevation[0], 17.0);
	EXPECT_DOUBLE_EQ(resp.elevation[2], 100.25);
}

TEST(GlazeDeserializerTest, ParsesGeocodingWithOptionalFields) {
	const std::string body = R"({
		"results": [
			{
				"id": 5128581,
				"name": "New York",
				"latitude": 40.7128,
				"longitude": -74.006,
				"elevation": 10.0,
				"timezone": "America/New_York",
				"country": "United States",
				"country_code": "US",
				"admin1": "New York",
				"population": 8175133,
				"postcodes": ["10001", "10002"]
			},
			{
				"id": 5128582,
				"name": "Newark",
				"latitude": 40.7357,
				"longitude": -74.1724,
				"elevation": 5.0,
				"timezone": "America/New_York",
				"country": "United States",
				"country_code": "US"
			}
		],
		"generationtime_ms": 0.31
	})";

	GeocodingResponse resp;
	Result<void> r = deserialize_geocoding_response(body, resp);
	ASSERT_TRUE(r.has_value()) << (r ? "" : r.error().message);
	ASSERT_EQ(resp.results.size(), 2u);

	const GeocodingResult& ny = resp.results[0];
	EXPECT_EQ(ny.id, 5128581);
	EXPECT_EQ(ny.name, "New York");
	ASSERT_TRUE(ny.admin1.has_value());
	EXPECT_EQ(*ny.admin1, "New York");
	ASSERT_TRUE(ny.population.has_value());
	EXPECT_EQ(*ny.population, 8175133);
	ASSERT_TRUE(ny.postcodes.has_value());
	ASSERT_EQ(ny.postcodes->size(), 2u);
	EXPECT_EQ((*ny.postcodes)[0], "10001");

	const GeocodingResult& nk = resp.results[1];
	EXPECT_EQ(nk.id, 5128582);
	EXPECT_FALSE(nk.admin1.has_value());
	EXPECT_FALSE(nk.population.has_value());
	EXPECT_FALSE(nk.postcodes.has_value());
}

TEST(GlazeDeserializerTest, ParsesSeasonalExtras) {
	const std::string body = R"({
		"latitude": 40.0,
		"longitude": -74.0,
		"elevation": 5.0,
		"generationtime_ms": 1.0,
		"utc_offset_seconds": 0,
		"timezone": "GMT",
		"timezone_abbreviation": "GMT",
		"six_hourly": {"time": ["2026-05-11T00:00"], "temperature_2m": [10.0]},
		"weekly":     {"time": ["2026-W19"],         "temperature_2m_mean": [11.5]},
		"monthly":    {"time": ["2026-05"],          "temperature_2m_mean": [12.0]},
		"weekly_units":  {"temperature_2m_mean": "°C"}
	})";

	SeasonalResponse resp;
	Result<void> r = deserialize_seasonal_response(body, resp);
	ASSERT_TRUE(r.has_value()) << (r ? "" : r.error().message);

	ASSERT_TRUE(resp.six_hourly.has_value());
	ASSERT_EQ(resp.six_hourly->time.size(), 1u);
	ASSERT_TRUE(resp.weekly.has_value());
	EXPECT_EQ(resp.weekly->time[0], "2026-W19");
	ASSERT_TRUE(resp.monthly.has_value());

	ASSERT_TRUE(resp.weekly_units.has_value());
	EXPECT_EQ((*resp.weekly_units)["temperature_2m_mean"], "°C");

	// Standard WeatherResponse fields should be absent (we didn't include them)
	EXPECT_FALSE(resp.hourly.has_value());
	EXPECT_FALSE(resp.daily.has_value());
}

TEST(GlazeDeserializerTest, RejectsInvalidJson) {
	const std::string body = R"({"latitude": "this is not a number)";
	ForecastResponse resp;
	Result<void> r = deserialize_weather_response(body, resp);
	EXPECT_FALSE(r.has_value());
}

} // namespace
} // namespace open_meteo
