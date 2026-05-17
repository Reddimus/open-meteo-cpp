// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/params.hpp"

#include <gtest/gtest.h>

namespace open_meteo {
namespace {

TEST(PreviousRunsParamsTest, PrefersExplicitModelNameOverModels) {
	PreviousRunsParams params;
	params.latitude = 47.61;
	params.longitude = -122.33;
	params.start_date = "2026-04-20";
	params.end_date = "2026-04-20";
	params.hourly = std::vector<std::string>{"temperature_2m"};
	params.models = std::vector<std::string>{"best_match", "gfs_seamless"};
	params.model_name = "hrrr";

	const std::string query = params.build_query_string();

	EXPECT_NE(query.find("model_name=hrrr"), std::string::npos);
	EXPECT_EQ(query.find("models="), std::string::npos);
}

TEST(PreviousRunsParamsTest, NormalizesSingletonModelsToModelName) {
	PreviousRunsParams params;
	params.latitude = 47.61;
	params.longitude = -122.33;
	params.start_date = "2026-04-20";
	params.end_date = "2026-04-20";
	params.models = std::vector<std::string>{"nbm"};

	const std::string query = params.build_query_string();

	EXPECT_NE(query.find("model_name=nbm"), std::string::npos);
	EXPECT_EQ(query.find("models="), std::string::npos);
}

TEST(PreviousRunsParamsTest, KeepsMultiModelRequestsAsModelsList) {
	PreviousRunsParams params;
	params.latitude = 47.61;
	params.longitude = -122.33;
	params.start_date = "2026-04-20";
	params.end_date = "2026-04-20";
	params.models = std::vector<std::string>{"best_match", "gfs_seamless"};

	const std::string query = params.build_query_string();

	EXPECT_NE(query.find("models=best_match,gfs_seamless"), std::string::npos);
	EXPECT_EQ(query.find("model_name="), std::string::npos);
}

// Regression guard for the contract open-meteo-data's native per-member
// ensemble backend depends on: EnsembleParams must serialize a multi-model
// list, a >=16-day horizon, and per-member daily tmax/tmin.
TEST(EnsembleParamsTest, SerializesMultiModelDailyHorizon) {
	EnsembleParams params;
	params.latitude = 40.7128;
	params.longitude = -74.0060;
	params.models =
		std::vector<std::string>{"gfs_seamless", "ecmwf_ifs025", "icon_seamless", "gem_seamless"};
	params.forecast_days = 16;
	params.daily = std::vector<std::string>{"temperature_2m_max", "temperature_2m_min"};

	const std::string query = params.build_query_string();

	EXPECT_NE(query.find("models=gfs_seamless,ecmwf_ifs025,icon_seamless,gem_seamless"),
			  std::string::npos);
	EXPECT_NE(query.find("forecast_days=16"), std::string::npos);
	EXPECT_NE(query.find("daily=temperature_2m_max,temperature_2m_min"), std::string::npos);
}

} // namespace
} // namespace open_meteo
