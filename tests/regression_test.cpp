#include "open_meteo/client.hpp"
#include "open_meteo/models/common.hpp"
#include "open_meteo/models/elevation.hpp"
#include "open_meteo/models/geocoding.hpp"
#include "open_meteo/models/params.hpp"
#include "open_meteo/models/seasonal.hpp"
#include "open_meteo/rate_limit.hpp"
#include "open_meteo/retry.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <memory>

using namespace open_meteo;

TEST(Regression, CurrentConditionsPreserveScalarValues) {
	WeatherResponse out;
	ASSERT_TRUE(deserialize_weather_response(
		R"({"current":{"time":"2026-10-25T12:00","temperature_2m":18.5,"rain":null}})", out));
	ASSERT_TRUE(out.current);
	ASSERT_EQ(out.current->time.size(), 1);
	EXPECT_EQ(out.current->time.front(), "2026-10-25T12:00");
	ASSERT_EQ(out.current->values.at("temperature_2m").size(), 1);
	EXPECT_DOUBLE_EQ(out.current->values.at("temperature_2m").front(), 18.5);
	EXPECT_TRUE(std::isnan(out.current->values.at("rain").front()));
}

TEST(Regression, ReusedResponsesReplaceMissingFields) {
	WeatherResponse out;
	ASSERT_TRUE(deserialize_weather_response(R"({"latitude":42,"timezone":"UTC"})", out));
	ASSERT_TRUE(deserialize_weather_response("{}", out));
	EXPECT_DOUBLE_EQ(out.latitude, 0);
	EXPECT_TRUE(out.timezone.empty());
}

TEST(Regression, EveryParserRejectsTrailingAndMalformedUnknownJson) {
	for (const std::string& input :
		 {std::string("{} garbage"), std::string("{} {}"), std::string(R"({"unknown":tru})")}) {
		Location location;
		WeatherResponse weather;
		SeasonalResponse seasonal;
		ElevationResponse elevation;
		GeocodingResponse geocoding;
		EXPECT_FALSE(deserialize_location(input, location)) << input;
		EXPECT_FALSE(deserialize_weather_response(input, weather)) << input;
		EXPECT_FALSE(deserialize_seasonal_response(input, seasonal)) << input;
		EXPECT_FALSE(deserialize_elevation_response(input, elevation)) << input;
		EXPECT_FALSE(deserialize_geocoding_response(input, geocoding)) << input;
	}
}

TEST(Regression, BorrowedBuffersNeedNoTerminator) {
	const std::string input = R"({"unknown":[true,false,null,{"value":"\u1234"}]})";
	for (std::size_t length = 1; length <= input.size(); ++length) {
		std::unique_ptr<char[]> bytes = std::make_unique<char[]>(length);
		std::copy_n(input.data(), length, bytes.get());
		Location out;
		const Result<void> result = deserialize_location({bytes.get(), length}, out);
		EXPECT_EQ(result.has_value(), length == input.size());
	}
}

TEST(Regression, QueryValuesCannotInjectParameters) {
	std::string query;
	append_param(query, "name", std::string("New York&count=100#x"));
	EXPECT_EQ(query, "?name=New%20York%26count%3D100%23x");
}

TEST(Regression, ZeroWaitCanUseAnAvailableToken) {
	RateLimiter limiter({.max_tokens = 1, .initial_tokens = 1});
	EXPECT_TRUE(limiter.acquire_for(std::chrono::milliseconds::zero()));
}

TEST(Regression, InvalidRateSettingsAreNormalized) {
	RateLimiter limiter({.max_tokens = 0,
						 .refill_interval = std::chrono::milliseconds::zero(),
						 .initial_tokens = 5});
	EXPECT_GE(limiter.config().max_tokens, 1);
	EXPECT_GT(limiter.config().refill_interval.count(), 0);
	EXPECT_LE(limiter.available_tokens(), limiter.config().max_tokens);
}

TEST(Regression, RetryJitterNeverExceedsCap) {
	RetryPolicy policy{.initial_delay = std::chrono::milliseconds{100},
					   .max_delay = std::chrono::milliseconds{100},
					   .jitter_factor = 1};
	for (int i = 0; i < 100; ++i) {
		EXPECT_LE(calculate_retry_delay(2, policy), policy.max_delay);
	}
}

TEST(Regression, StringLiteralsAreNotConvertedToBool) {
	std::string query;
	append_param(query, "name", "New York");
	EXPECT_EQ(query, "?name=New%20York");
}
TEST(Regression, ExtremeRetrySettingsStayWithinBounds) {
	RetryPolicy policy{.initial_delay = std::chrono::milliseconds::max(),
					   .max_delay = std::chrono::milliseconds::max(),
					   .backoff_multiplier = std::numeric_limits<double>::infinity(),
					   .jitter_factor = std::numeric_limits<double>::quiet_NaN()};
	EXPECT_EQ(calculate_retry_delay(255, policy), policy.max_delay);
	policy.max_delay = std::chrono::milliseconds{-1};
	EXPECT_EQ(calculate_retry_delay(255, policy), std::chrono::milliseconds::zero());
}

TEST(Regression, MovedFromApiClientsReturnErrors) {
	OpenMeteoClient first(OpenMeteoClient::Config{});
	OpenMeteoClient second(std::move(first));
	EXPECT_FALSE(first.get_elevation({}));
	EXPECT_THROW(first.http_client(), std::logic_error);
}
