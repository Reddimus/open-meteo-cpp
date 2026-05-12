// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT

#include "open_meteo/client.hpp"

#include <string>
#include <string_view>

namespace open_meteo {

// ===== Client Impl =====

struct OpenMeteoClient::Impl {
	Config config;
	HttpClient http;
	RateLimiter rate_limiter;
	RetryPolicy retry_policy;

	explicit Impl(Config cfg)
		: config(std::move(cfg)), http(config.http), rate_limiter(config.rate_limit),
		  retry_policy(config.retry) {}
};

OpenMeteoClient::OpenMeteoClient(Config config)
	: impl_(std::make_unique<Impl>(std::move(config))) {}

OpenMeteoClient::~OpenMeteoClient() = default;
OpenMeteoClient::OpenMeteoClient(OpenMeteoClient&&) noexcept = default;
OpenMeteoClient& OpenMeteoClient::operator=(OpenMeteoClient&&) noexcept = default;

HttpClient& OpenMeteoClient::http_client() {
	return impl_->http;
}

const HttpClient& OpenMeteoClient::http_client() const {
	return impl_->http;
}

std::string OpenMeteoClient::resolve_url(std::string_view base_url) const {
	std::string url(base_url);
	if (impl_->config.use_customer_endpoints) {
		// Replace "https://" prefix with "https://customer-"
		// e.g. "https://api.open-meteo.com" -> "https://customer-api.open-meteo.com"
		constexpr std::string_view https_prefix = "https://";
		if (url.starts_with(https_prefix)) {
			url.insert(https_prefix.size(), "customer-");
		}
	}
	return url;
}

Result<HttpResponse> OpenMeteoClient::do_get(const std::string& full_url) {
	(void)impl_->rate_limiter.acquire();

	// The HttpClient expects a path relative to base_url, but for Open-Meteo
	// we need full URLs since each endpoint has a different base. We pass the
	// full URL minus the configured base_url prefix, OR we construct the path
	// ourselves. Since each endpoint has a unique domain, we pass the complete
	// URL as the path and rely on HttpClient.get() which prepends base_url.
	//
	// Strategy: strip the HttpClient's base_url from the front of full_url,
	// or if different domain, set path to the full URL (HttpClient handles
	// base_url="" case by using path as-is).
	//
	// Since we configure HttpClient with an empty base_url, we pass the full URL.
	return with_retry([&]() { return impl_->http.get(full_url); }, impl_->retry_policy);
}

// ===== Endpoint implementations =====
// Each follows the pattern:
// 1. Build query string from params
// 2. Construct full URL (base + query)
// 3. HTTP GET with rate limiting + retry (via do_get)
// 4. Glaze-parse JSON response via the appropriate deserialize_* helper
// 5. Return Result<T>

namespace {

// Common HTTP-result handling: status check + body extraction.
// Returns the body on success, an Error on failure.
Result<std::string> body_or_error(Result<HttpResponse>&& http_result) {
	if (!http_result) {
		return std::unexpected(http_result.error());
	}
	if (http_result->status_code != 200) {
		return std::unexpected(Error::from_response(http_result->status_code, http_result->body));
	}
	return std::move(http_result->body);
}

} // namespace

// ===== Forecast API =====

Result<ForecastResponse> OpenMeteoClient::get_forecast(const ForecastParams& params) {
	std::string full_url = resolve_url(url::kForecast) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	ForecastResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Historical API =====

Result<HistoricalWeatherResponse> OpenMeteoClient::get_historical(const HistoricalParams& params) {
	std::string full_url = resolve_url(url::kHistorical) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	HistoricalWeatherResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

Result<HistoricalForecastResponse>
OpenMeteoClient::get_historical_forecast(const HistoricalForecastParams& params) {
	std::string full_url = resolve_url(url::kHistoricalForecast) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	HistoricalForecastResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

Result<PreviousRunsResponse> OpenMeteoClient::get_previous_runs(const PreviousRunsParams& params) {
	std::string full_url = resolve_url(url::kPreviousRuns) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	PreviousRunsResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Ensemble API =====

Result<EnsembleResponse> OpenMeteoClient::get_ensemble(const EnsembleParams& params) {
	std::string full_url = resolve_url(url::kEnsemble) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	EnsembleResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Marine API =====

Result<MarineResponse> OpenMeteoClient::get_marine(const MarineParams& params) {
	std::string full_url = resolve_url(url::kMarine) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	MarineResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Air Quality API =====

Result<AirQualityResponse> OpenMeteoClient::get_air_quality(const AirQualityParams& params) {
	std::string full_url = resolve_url(url::kAirQuality) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	AirQualityResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Climate API =====

Result<ClimateResponse> OpenMeteoClient::get_climate(const ClimateParams& params) {
	std::string full_url = resolve_url(url::kClimate) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	ClimateResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Seasonal API =====

Result<SeasonalResponse> OpenMeteoClient::get_seasonal(const SeasonalParams& params) {
	std::string full_url = resolve_url(url::kSeasonal) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	SeasonalResponse response;
	Result<void> parse = deserialize_seasonal_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Flood API =====

Result<FloodResponse> OpenMeteoClient::get_flood(const FloodParams& params) {
	std::string full_url = resolve_url(url::kFlood) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	FloodResponse response;
	Result<void> parse = deserialize_weather_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Elevation API =====

Result<ElevationResponse> OpenMeteoClient::get_elevation(const ElevationParams& params) {
	std::string full_url = resolve_url(url::kElevation) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	ElevationResponse response;
	Result<void> parse = deserialize_elevation_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

// ===== Geocoding API =====

Result<GeocodingResponse> OpenMeteoClient::get_geocoding(const GeocodingParams& params) {
	std::string full_url = resolve_url(url::kGeocoding) + params.build_query_string();
	Result<std::string> body = body_or_error(do_get(full_url));
	if (!body) {
		return std::unexpected(body.error());
	}
	GeocodingResponse response;
	Result<void> parse = deserialize_geocoding_response(*body, response);
	if (!parse) {
		return std::unexpected(parse.error());
	}
	return response;
}

} // namespace open_meteo
