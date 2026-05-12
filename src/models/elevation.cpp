// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/models/elevation.hpp"

#include <glaze/glaze.hpp>
#include <string_view>

namespace open_meteo {

// ElevationResponse is fully-statically-typed (single double array). The
// payload has no dynamic keys, so we use Glaze's direct reflected path for
// maximum throughput.

} // namespace open_meteo

template <>
struct glz::meta<open_meteo::ElevationResponse> {
	using T = open_meteo::ElevationResponse;
	static constexpr auto value = object("elevation", &T::elevation); // auto-ok: glz::object
};

namespace open_meteo {

Result<void> deserialize_elevation_response(std::string_view body, ElevationResponse& out) {
	constexpr glz::opts opts{.error_on_unknown_keys = false};
	glz::error_ctx ec = glz::read<opts>(out, body);
	if (ec) {
		return std::unexpected(Error::parse(glz::format_error(ec, body)));
	}
	return {};
}

} // namespace open_meteo
