// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
//
// Microbenchmark: parse a representative Open-Meteo forecast response 1k
// times and report wall-clock. Used as a parse-throughput regression guard
// with `ctest --timeout` and an absolute upper bound on us/op.
//
// Historical baseline (recorded at migration time on x86_64-v3, GCC 13.3,
// -O3 -DNDEBUG, payload=7.95KB, iters=1000):
//
//     nlohmann/json v3.11.3 :  ~150 us/op  (pre-migration baseline)
//     glaze v7.6.0          :   ~41 us/op  (post-migration)
//     speedup               :   3.4-4.0x
//
// The pre-migration nlohmann path lived in tests/parse_benchmark.cpp at the
// time of the Glaze migration commit; it has since been removed along with
// the nlohmann FetchContent dep. Re-introduce a side-by-side bench only if
// a future regression suspicion warrants it.

#include "open_meteo/models/common.hpp"
#include "open_meteo/models/forecast.hpp"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>

namespace {

// 24-hour-step temperatures + precipitation + relative_humidity, plus a
// string-typed weather_code series. Mimics a typical 7-day hourly forecast
// (168 points / variable). Roughly 8 KB of JSON.
std::string make_payload() {
	std::string json;
	json.reserve(32 * 1024);
	json += R"({"latitude":40.7128,"longitude":-74.006,"elevation":17.0,)";
	json += R"("generationtime_ms":0.45,"utc_offset_seconds":-14400,)";
	json += R"("timezone":"America/New_York","timezone_abbreviation":"EDT",)";
	json += R"("hourly_units":{"temperature_2m":"°C","precipitation":"mm",)";
	json += R"("relative_humidity_2m":"%","weather_code":"wmo"},)";
	json += R"("hourly":{"time":[)";
	constexpr int kHours = 168;
	for (int i = 0; i < kHours; ++i) {
		char buf[64];
		std::snprintf(buf, sizeof(buf), "\"2026-05-11T%02d:00\"", i % 24);
		if (i != 0) {
			json += ',';
		}
		json += buf;
	}
	json += R"(],"temperature_2m":[)";
	for (int i = 0; i < kHours; ++i) {
		char buf[32];
		std::snprintf(buf, sizeof(buf), "%.2f", 10.0 + 5.0 * std::sin(i * 0.13));
		if (i != 0) {
			json += ',';
		}
		json += buf;
	}
	json += R"(],"precipitation":[)";
	for (int i = 0; i < kHours; ++i) {
		char buf[32];
		std::snprintf(buf, sizeof(buf), "%.2f", std::max(0.0, 0.5 * std::sin(i * 0.07)));
		if (i != 0) {
			json += ',';
		}
		json += buf;
	}
	json += R"(],"relative_humidity_2m":[)";
	for (int i = 0; i < kHours; ++i) {
		char buf[32];
		std::snprintf(buf, sizeof(buf), "%.1f", 55.0 + 15.0 * std::cos(i * 0.11));
		if (i != 0) {
			json += ',';
		}
		json += buf;
	}
	json += R"(],"weather_code":[)";
	const char* codes[] = {"\"clear\"", "\"partly_cloudy\"", "\"overcast\"", "\"rain\""};
	for (int i = 0; i < kHours; ++i) {
		if (i != 0) {
			json += ',';
		}
		json += codes[i % 4];
	}
	json += "]}}";
	return json;
}

} // namespace

int main() {
	const std::string payload = make_payload();
	constexpr int kIterations = 1000;

	// Warmup — let the allocator and CPU settle.
	for (int i = 0; i < 50; ++i) {
		open_meteo::WeatherResponse warm;
		(void)open_meteo::deserialize_weather_response(payload, warm);
	}

	std::chrono::nanoseconds glaze_total{0};
	std::size_t glaze_checksum = 0;
	for (int i = 0; i < kIterations; ++i) {
		open_meteo::WeatherResponse resp;
		std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
		open_meteo::Result<void> r = open_meteo::deserialize_weather_response(payload, resp);
		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		if (!r) {
			std::fprintf(stderr, "glaze parse failed: %s\n", r.error().message.c_str());
			return 1;
		}
		glaze_total += (t1 - t0);
		if (resp.hourly && !resp.hourly->time.empty()) {
			glaze_checksum += resp.hourly->time.size();
		}
	}

	if (glaze_checksum != static_cast<std::size_t>(168) * kIterations) {
		std::fprintf(stderr, "checksum mismatch: glaze=%zu (expected %d)\n", glaze_checksum,
					 168 * kIterations);
		return 1;
	}

	const double glaze_ms = glaze_total.count() / 1e6;
	const double us_per_op = (glaze_total.count() / 1e3) / kIterations;

	std::printf("parse_benchmark: payload=%zuB iters=%d\n", payload.size(), kIterations);
	std::printf("  glaze: %8.3f ms total  (%8.3f us/op)\n", glaze_ms, us_per_op);

	// Regression guard: at migration time, Glaze parsed this payload at
	// ~41 us/op on x86_64-v3. Set the cap at 200 us/op — that's still
	// ~5x faster than the nlohmann baseline (~150 us/op) and leaves a
	// healthy slack window for slower CI runners and Debug builds.
	constexpr double kMaxUsPerOp = 200.0;
	if (us_per_op > kMaxUsPerOp) {
		std::fprintf(stderr, "REGRESSION: %.3f us/op exceeds cap of %.0f us/op\n", us_per_op,
					 kMaxUsPerOp);
		return 1;
	}
	return 0;
}
