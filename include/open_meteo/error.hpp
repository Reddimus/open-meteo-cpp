// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace open_meteo {

/// Error codes for Open-Meteo SDK operations
enum class ErrorCode : std::uint8_t {
	Ok = 0,
	NetworkError,
	RateLimited,
	ServerError,
	NotFound,
	InvalidRequest,
	ParseError,
	Timeout,
	Unknown
};

/// Convert ErrorCode to string
[[nodiscard]] constexpr std::string_view to_string(ErrorCode code) noexcept {
	switch (code) {
		case ErrorCode::Ok:
			return "Ok";
		case ErrorCode::NetworkError:
			return "NetworkError";
		case ErrorCode::RateLimited:
			return "RateLimited";
		case ErrorCode::ServerError:
			return "ServerError";
		case ErrorCode::NotFound:
			return "NotFound";
		case ErrorCode::InvalidRequest:
			return "InvalidRequest";
		case ErrorCode::ParseError:
			return "ParseError";
		case ErrorCode::Timeout:
			return "Timeout";
		case ErrorCode::Unknown:
			return "Unknown";
	}
	return "Unknown";
}

/// Error information returned by SDK operations
struct Error {
	ErrorCode code;
	std::string message;
	int http_status{0};
	std::string detail; // Open-Meteo "reason" field from error responses

	[[nodiscard]] constexpr bool is_ok() const noexcept { return code == ErrorCode::Ok; }

	[[nodiscard]] static Error ok() noexcept { return {ErrorCode::Ok, "", 0, {}}; }

	[[nodiscard]] static Error network(std::string msg) {
		return {ErrorCode::NetworkError, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error parse(std::string msg) {
		return {ErrorCode::ParseError, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error not_found(std::string msg) {
		return {ErrorCode::NotFound, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error rate_limited(std::string msg) {
		return {ErrorCode::RateLimited, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error server(std::string msg) {
		return {ErrorCode::ServerError, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error invalid_request(std::string msg) {
		return {ErrorCode::InvalidRequest, std::move(msg), 0, {}};
	}

	[[nodiscard]] static Error timeout(std::string msg) {
		return {ErrorCode::Timeout, std::move(msg), 0, {}};
	}

	/// Create an Error from an HTTP response status code and body
	[[nodiscard]] static Error from_response(int status, const std::string& body);
};

/// Result type for SDK operations
template <typename T>
using Result = std::expected<T, Error>;

} // namespace open_meteo
