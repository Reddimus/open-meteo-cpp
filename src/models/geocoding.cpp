// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/geocoding.hpp"

#include <glaze/glaze.hpp>
#include <string_view>

namespace open_meteo {

// GeocodingResult / GeocodingResponse are fully-statically-typed. Use Glaze's
// direct reflected path. Open-Meteo occasionally returns extra metadata fields
// (e.g. feature_code, admin*_id) that we don't model — tolerate them via
// error_on_unknown_keys = false.

} // namespace open_meteo

template <>
struct glz::meta<open_meteo::GeocodingResult> {
	using T = open_meteo::GeocodingResult;
	static constexpr auto value = // auto-ok: glz::object returns unspellable tuple
		object("id", &T::id, "name", &T::name, "latitude", &T::latitude, "longitude", &T::longitude,
			   "elevation", &T::elevation, "timezone", &T::timezone, "country", &T::country,
			   "country_code", &T::country_code, "admin1", &T::admin1, "admin2", &T::admin2,
			   "admin3", &T::admin3, "population", &T::population, "postcodes", &T::postcodes);
};

template <>
struct glz::meta<open_meteo::GeocodingResponse> {
	using T = open_meteo::GeocodingResponse;
	static constexpr auto value = // auto-ok: glz::object returns unspellable tuple
		object("results", &T::results, "generationtime_ms", &T::generationtime_ms);
};

namespace open_meteo {

Result<void> deserialize_geocoding_response(std::string_view body, GeocodingResponse& out) {
	constexpr glz::opts opts{.error_on_unknown_keys = false};
	glz::error_ctx ec = glz::read<opts>(out, body);
	if (ec) {
		return std::unexpected(Error::parse(glz::format_error(ec, body)));
	}
	return {};
}

} // namespace open_meteo
