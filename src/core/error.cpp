// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/error.hpp"

#include <nlohmann/json.hpp>

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

	// Try to parse Open-Meteo error JSON response
	// Format: {"error": true, "reason": "..."}
	try {
		nlohmann::json j = nlohmann::json::parse(body);
		if (j.contains("reason") && j["reason"].is_string()) {
			err.message = j["reason"].get<std::string>();
			err.detail = err.message;
		}
	} catch (...) {
		// Not valid JSON -- use raw body as message
		err.message = body.substr(0, 256);
	}

	if (err.message.empty()) {
		err.message = "HTTP " + std::to_string(status);
	}

	return err;
}

} // namespace open_meteo
