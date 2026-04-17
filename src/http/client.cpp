// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/http_client.hpp"

#include <curl/curl.h>

namespace open_meteo {

static size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
	std::string* body = static_cast<std::string*>(userdata);
	body->append(static_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
}

static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
	std::vector<std::pair<std::string, std::string>>* headers =
		static_cast<std::vector<std::pair<std::string, std::string>>*>(userdata);
	std::string line(buffer, size * nitems);

	std::size_t colon = line.find(':');
	if (colon != std::string::npos) {
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		// Trim whitespace
		std::size_t start = value.find_first_not_of(" \t\r\n");
		std::size_t end = value.find_last_not_of(" \t\r\n");
		if (start != std::string::npos && end != std::string::npos) {
			value = value.substr(start, end - start + 1);
		}
		headers->emplace_back(std::move(key), std::move(value));
	}

	return size * nitems;
}

struct HttpClient::Impl {
	HttpClientConfig config;
	CURL* curl{nullptr};

	Impl(HttpClientConfig cfg) : config(std::move(cfg)) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();
	}

	~Impl() {
		if (curl) {
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();
	}

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;

	/// Build the full URL, appending api_key as a query parameter if set.
	/// If path is already an absolute URL (http:// or https://), it is used as-is.
	[[nodiscard]] std::string build_url(std::string_view path) const {
		std::string url;
		if (path.starts_with("http://") || path.starts_with("https://")) {
			url.assign(path);
		} else {
			url = config.base_url + std::string(path);
		}

		if (!config.api_key.empty()) {
			// Determine separator: '?' if no query string yet, '&' otherwise
			char separator = (url.find('?') == std::string::npos) ? '?' : '&';
			url += separator;
			url += "apikey=";
			url += config.api_key;
		}

		return url;
	}
};

HttpClient::HttpClient(HttpClientConfig config)
	: impl_(std::make_unique<Impl>(std::move(config))) {}

HttpClient::~HttpClient() = default;

HttpClient::HttpClient(HttpClient&&) noexcept = default;
HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

Result<HttpResponse> HttpClient::get(std::string_view path) const {
	if (!impl_->curl) {
		return std::unexpected(Error::network("CURL not initialized"));
	}

	CURL* curl = impl_->curl;
	std::string url = impl_->build_url(path);
	std::string body;
	std::vector<std::pair<std::string, std::string>> response_headers;

	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, impl_->config.timeout.count());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	// Set User-Agent
	if (!impl_->config.user_agent.empty()) {
		curl_easy_setopt(curl, CURLOPT_USERAGENT, impl_->config.user_agent.c_str());
	}

	// Accept JSON responses
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Accept: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// SSL verification
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, impl_->config.verify_ssl ? 1L : 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, impl_->config.verify_ssl ? 2L : 0L);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);

	if (res == CURLE_OPERATION_TIMEDOUT) {
		return std::unexpected(Error::timeout(curl_easy_strerror(res)));
	}

	if (res != CURLE_OK) {
		return std::unexpected(Error::network(curl_easy_strerror(res)));
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	return HttpResponse{
		static_cast<std::int16_t>(http_code),
		std::move(body),
		std::move(response_headers),
	};
}

const HttpClientConfig& HttpClient::config() const noexcept {
	return impl_->config;
}

} // namespace open_meteo
