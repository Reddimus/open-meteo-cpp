// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

/// @file client.hpp
/// @brief Complete REST API client for Open-Meteo
///
/// Provides typed methods for all 12 Open-Meteo API endpoints.
/// Uses Pimpl pattern for ABI stability. Move-only, non-copyable.

#include "open_meteo/error.hpp"
#include "open_meteo/http_client.hpp"
#include "open_meteo/models/air_quality.hpp"
#include "open_meteo/models/climate.hpp"
#include "open_meteo/models/elevation.hpp"
#include "open_meteo/models/ensemble.hpp"
#include "open_meteo/models/flood.hpp"
#include "open_meteo/models/forecast.hpp"
#include "open_meteo/models/geocoding.hpp"
#include "open_meteo/models/historical.hpp"
#include "open_meteo/models/marine.hpp"
#include "open_meteo/models/params.hpp"
#include "open_meteo/models/seasonal.hpp"
#include "open_meteo/rate_limit.hpp"
#include "open_meteo/retry.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace open_meteo {

// ===== Base URLs =====

namespace url {

inline constexpr std::string_view kForecast = "https://api.open-meteo.com/v1/forecast";
inline constexpr std::string_view kHistorical = "https://archive-api.open-meteo.com/v1/archive";
inline constexpr std::string_view kHistoricalForecast =
	"https://historical-forecast-api.open-meteo.com/v1/forecast";
inline constexpr std::string_view kPreviousRuns =
	"https://previous-runs-api.open-meteo.com/v1/forecast";
inline constexpr std::string_view kEnsemble = "https://ensemble-api.open-meteo.com/v1/ensemble";
inline constexpr std::string_view kMarine = "https://api.open-meteo.com/v1/marine";
inline constexpr std::string_view kAirQuality =
	"https://air-quality-api.open-meteo.com/v1/air-quality";
inline constexpr std::string_view kClimate = "https://climate-api.open-meteo.com/v1/climate";
inline constexpr std::string_view kSeasonal = "https://seasonal-api.open-meteo.com/v1/seasonal";
inline constexpr std::string_view kFlood = "https://flood-api.open-meteo.com/v1/flood";
inline constexpr std::string_view kElevation = "https://api.open-meteo.com/v1/elevation";
inline constexpr std::string_view kGeocoding = "https://geocoding-api.open-meteo.com/v1/search";

} // namespace url

// ===== Client =====

/// Complete Open-Meteo API client
///
/// Provides typed methods for all 12 Open-Meteo endpoints with rate limiting,
/// retry, and optional API key injection for customer endpoints.
///
/// Uses the existing HttpClient, RateLimiter, and RetryPolicy infrastructure.
///
/// @note Thread Safety: NOT thread-safe. Create one client per thread or
/// protect access with a mutex.
class OpenMeteoClient {
public:
	/// Client configuration
	struct Config {
		HttpClientConfig http{};
		RateLimiter::Config rate_limit{};
		RetryPolicy retry{};
		bool use_customer_endpoints{false}; ///< Prepend "customer-" to subdomains
	};

	explicit OpenMeteoClient(Config config);
	~OpenMeteoClient();

	OpenMeteoClient(OpenMeteoClient&&) noexcept;
	OpenMeteoClient& operator=(OpenMeteoClient&&) noexcept;

	// Non-copyable
	OpenMeteoClient(const OpenMeteoClient&) = delete;
	OpenMeteoClient& operator=(const OpenMeteoClient&) = delete;

	// ===== Forecast API =====

	/// Get weather forecast
	[[nodiscard]] Result<ForecastResponse> get_forecast(const ForecastParams& params);

	// ===== Historical API =====

	/// Get historical weather data from the archive
	[[nodiscard]] Result<HistoricalWeatherResponse> get_historical(const HistoricalParams& params);

	/// Get historical forecast model data
	[[nodiscard]] Result<HistoricalForecastResponse>
	get_historical_forecast(const HistoricalForecastParams& params);

	/// Get data from previous model runs
	[[nodiscard]] Result<PreviousRunsResponse> get_previous_runs(const PreviousRunsParams& params);

	// ===== Ensemble API =====

	/// Get ensemble model forecast
	[[nodiscard]] Result<EnsembleResponse> get_ensemble(const EnsembleParams& params);

	// ===== Marine API =====

	/// Get marine weather forecast
	[[nodiscard]] Result<MarineResponse> get_marine(const MarineParams& params);

	// ===== Air Quality API =====

	/// Get air quality data
	[[nodiscard]] Result<AirQualityResponse> get_air_quality(const AirQualityParams& params);

	// ===== Climate API =====

	/// Get climate change projection data
	[[nodiscard]] Result<ClimateResponse> get_climate(const ClimateParams& params);

	// ===== Seasonal API =====

	/// Get seasonal forecast
	[[nodiscard]] Result<SeasonalResponse> get_seasonal(const SeasonalParams& params);

	// ===== Flood API =====

	/// Get flood forecast
	[[nodiscard]] Result<FloodResponse> get_flood(const FloodParams& params);

	// ===== Elevation API =====

	/// Get elevation for coordinates
	[[nodiscard]] Result<ElevationResponse> get_elevation(const ElevationParams& params);

	// ===== Geocoding API =====

	/// Search for locations by name
	[[nodiscard]] Result<GeocodingResponse> get_geocoding(const GeocodingParams& params);

	// ===== Accessors =====

	/// Access the underlying HTTP client
	[[nodiscard]] HttpClient& http_client();
	[[nodiscard]] const HttpClient& http_client() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;

	/// Resolve the base URL for an endpoint, handling customer endpoint switching
	[[nodiscard]] std::string resolve_url(std::string_view base_url) const;

	/// Perform a GET request with rate limiting and retry
	[[nodiscard]] Result<HttpResponse> do_get(const std::string& full_url);
};

} // namespace open_meteo
