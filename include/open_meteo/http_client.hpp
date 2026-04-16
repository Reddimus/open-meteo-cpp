// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

#include "open_meteo/error.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace open_meteo {

/// HTTP response
/// Uses contiguous vector storage for headers instead of unordered_map
/// for better cache locality (typically <10 headers in a response).
struct HttpResponse {
	std::int16_t status_code; // HTTP status codes fit in int16 (100-599)
	std::string body;
	std::vector<std::pair<std::string, std::string>> headers;
};

/// HTTP client configuration
struct HttpClientConfig {
	std::string base_url{"https://api.open-meteo.com"};
	std::string user_agent{"open-meteo-cpp/0.1.0"};
	std::string api_key;				   // Optional: for Open-Meteo commercial API
	std::chrono::seconds timeout{30};
	bool verify_ssl{true};
};

/// HTTP client for Open-Meteo API
///
/// GET-only client since the Open-Meteo API is read-only.
///
/// @note Thread Safety: This class is NOT thread-safe. The underlying CURL
/// handle is shared across all requests. If you need concurrent API access,
/// create one HttpClient instance per thread or protect access with a mutex.
class HttpClient {
public:
	/// Create a client with the given configuration
	explicit HttpClient(HttpClientConfig config);
	~HttpClient();

	HttpClient(HttpClient&&) noexcept;
	HttpClient& operator=(HttpClient&&) noexcept;

	// Non-copyable
	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;

	/// Perform a GET request
	/// @param path URL path with query parameters (e.g. "/v1/forecast?latitude=52.52")
	[[nodiscard]] Result<HttpResponse> get(std::string_view path) const;

	/// Get the client configuration
	[[nodiscard]] const HttpClientConfig& config() const noexcept;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace open_meteo
