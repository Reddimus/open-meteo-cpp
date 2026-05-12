// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/error.hpp"

#include <glaze/glaze.hpp>
#include <glaze/json/generic.hpp>

namespace open_meteo {

Error Error::from_response(int status, const std::string& body) {
	Error err;
	err.http_status = status;

	// Determine error code from HTTP status
	if (status == 404) {
		err.code = ErrorCode::NotFound;
	} else if (status == 400) {
		err.code = ErrorCode::InvalidRequest;
	} else if (status == 429) {
		err.code = ErrorCode::RateLimited;
	} else if (status >= 500) {
		err.code = ErrorCode::ServerError;
	} else {
		err.code = ErrorCode::Unknown;
	}

	// Try to parse Open-Meteo error JSON response (Glaze).
	// Format: {"error": true, "reason": "..."}
	glz::generic root{};
	glz::error_ctx ec = glz::read_json(root, body);
	if (!ec && root.is_object()) {
		const glz::generic::object_t& obj = root.get_object();
		glz::generic::object_t::const_iterator reason_it = obj.find("reason");
		if (reason_it != obj.end() && reason_it->second.is_string()) {
			err.message = reason_it->second.get<std::string>();
			err.detail = err.message;
		}
	} else {
		// Not valid JSON -- use raw body as message
		err.message = body.substr(0, 256);
	}

	if (err.message.empty()) {
		err.message = "HTTP " + std::to_string(status);
	}

	return err;
}

} // namespace open_meteo
