#include <open_meteo/open_meteo.hpp>
int main() {
	open_meteo::WeatherResponse response;
	const open_meteo::Result<void> result = open_meteo::deserialize_weather_response(
		R"({"current":{"time":"2026-10-25T12:00","temperature_2m":18.5}})", response);
	return result && response.current &&
				   response.current->values.at("temperature_2m").front() == 18.5
			   ? 0
			   : 1;
}
