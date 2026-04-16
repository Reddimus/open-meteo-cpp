// Copyright (c) 2026 PredictionMarketsAI
// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <string_view>

namespace open_meteo {

/// Temperature unit for API requests
enum class TemperatureUnit : std::uint8_t {
	Celsius,
	Fahrenheit
};

/// Wind speed unit for API requests
enum class WindSpeedUnit : std::uint8_t {
	Kmh,
	Ms,
	Mph,
	Knots
};

/// Precipitation unit for API requests
enum class PrecipitationUnit : std::uint8_t {
	Mm,
	Inch
};

/// Time format for API responses
enum class TimeFormat : std::uint8_t {
	Iso8601,
	Unixtime
};

/// Grid cell selection strategy
enum class CellSelection : std::uint8_t {
	Land,
	Sea,
	Nearest
};

// --- Display string conversions ---

/// Convert TemperatureUnit to display string
[[nodiscard]] constexpr std::string_view to_string(TemperatureUnit unit) noexcept {
	switch (unit) {
		case TemperatureUnit::Celsius:
			return "Celsius";
		case TemperatureUnit::Fahrenheit:
			return "Fahrenheit";
	}
	return "Celsius";
}

/// Convert WindSpeedUnit to display string
[[nodiscard]] constexpr std::string_view to_string(WindSpeedUnit unit) noexcept {
	switch (unit) {
		case WindSpeedUnit::Kmh:
			return "km/h";
		case WindSpeedUnit::Ms:
			return "m/s";
		case WindSpeedUnit::Mph:
			return "mph";
		case WindSpeedUnit::Knots:
			return "knots";
	}
	return "km/h";
}

/// Convert PrecipitationUnit to display string
[[nodiscard]] constexpr std::string_view to_string(PrecipitationUnit unit) noexcept {
	switch (unit) {
		case PrecipitationUnit::Mm:
			return "mm";
		case PrecipitationUnit::Inch:
			return "inch";
	}
	return "mm";
}

/// Convert TimeFormat to display string
[[nodiscard]] constexpr std::string_view to_string(TimeFormat fmt) noexcept {
	switch (fmt) {
		case TimeFormat::Iso8601:
			return "ISO 8601";
		case TimeFormat::Unixtime:
			return "Unixtime";
	}
	return "ISO 8601";
}

/// Convert CellSelection to display string
[[nodiscard]] constexpr std::string_view to_string(CellSelection sel) noexcept {
	switch (sel) {
		case CellSelection::Land:
			return "land";
		case CellSelection::Sea:
			return "sea";
		case CellSelection::Nearest:
			return "nearest";
	}
	return "land";
}

// --- URL query parameter value conversions ---

/// Convert TemperatureUnit to Open-Meteo query parameter value
[[nodiscard]] constexpr std::string_view to_query_value(TemperatureUnit unit) noexcept {
	switch (unit) {
		case TemperatureUnit::Celsius:
			return "celsius";
		case TemperatureUnit::Fahrenheit:
			return "fahrenheit";
	}
	return "celsius";
}

/// Convert WindSpeedUnit to Open-Meteo query parameter value
[[nodiscard]] constexpr std::string_view to_query_value(WindSpeedUnit unit) noexcept {
	switch (unit) {
		case WindSpeedUnit::Kmh:
			return "kmh";
		case WindSpeedUnit::Ms:
			return "ms";
		case WindSpeedUnit::Mph:
			return "mph";
		case WindSpeedUnit::Knots:
			return "kn";
	}
	return "kmh";
}

/// Convert PrecipitationUnit to Open-Meteo query parameter value
[[nodiscard]] constexpr std::string_view to_query_value(PrecipitationUnit unit) noexcept {
	switch (unit) {
		case PrecipitationUnit::Mm:
			return "mm";
		case PrecipitationUnit::Inch:
			return "inch";
	}
	return "mm";
}

/// Convert TimeFormat to Open-Meteo query parameter value
[[nodiscard]] constexpr std::string_view to_query_value(TimeFormat fmt) noexcept {
	switch (fmt) {
		case TimeFormat::Iso8601:
			return "iso8601";
		case TimeFormat::Unixtime:
			return "unixtime";
	}
	return "iso8601";
}

/// Convert CellSelection to Open-Meteo query parameter value
[[nodiscard]] constexpr std::string_view to_query_value(CellSelection sel) noexcept {
	switch (sel) {
		case CellSelection::Land:
			return "land";
		case CellSelection::Sea:
			return "sea";
		case CellSelection::Nearest:
			return "nearest";
	}
	return "land";
}

} // namespace open_meteo
