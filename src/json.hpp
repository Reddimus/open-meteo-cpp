#pragma once
#include <glaze/glaze.hpp>
namespace open_meteo::detail {
struct JsonReadOptions : glz::opts {
	bool validate_trailing_whitespace = true;
	bool validate_skipped = true;
};
inline constexpr JsonReadOptions kJsonReadOptions{
	{.null_terminated = false, .error_on_unknown_keys = false}};
} // namespace open_meteo::detail
