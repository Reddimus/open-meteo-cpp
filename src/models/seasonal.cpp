// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/seasonal.hpp"

#include "open_meteo/models/common.hpp"

#include <glaze/glaze.hpp>
#include <glaze/json/generic.hpp>

#include "common_glaze_detail.hpp"

namespace open_meteo {

Result<void> deserialize_seasonal_response(std::string_view body, SeasonalResponse& out) {
	glz::generic root{};
	glz::error_ctx ec = glz::read_json(root, body);
	if (ec) {
		return std::unexpected(Error::parse(glz::format_error(ec, body)));
	}

	// Base Location + standard WeatherResponse timeseries blocks
	detail::populate_location(root, static_cast<Location&>(out));
	out.hourly = detail::grab_timeseries(root, "hourly");
	out.daily = detail::grab_timeseries(root, "daily");
	out.current = detail::grab_timeseries(root, "current");
	out.hourly_units = detail::grab_units_map(root, "hourly_units");
	out.daily_units = detail::grab_units_map(root, "daily_units");
	out.current_units = detail::grab_units_map(root, "current_units");

	// Seasonal-specific extensions
	out.six_hourly = detail::grab_timeseries(root, "six_hourly");
	out.weekly = detail::grab_timeseries(root, "weekly");
	out.monthly = detail::grab_timeseries(root, "monthly");
	out.six_hourly_units = detail::grab_units_map(root, "six_hourly_units");
	out.weekly_units = detail::grab_units_map(root, "weekly_units");
	out.monthly_units = detail::grab_units_map(root, "monthly_units");

	return {};
}

} // namespace open_meteo
