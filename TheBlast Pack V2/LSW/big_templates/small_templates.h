#pragma once

#include <type_traits>

#define BLACK	al_map_rgb(0,0,0)
#define WHITE	al_map_rgb(255,255,255)
#define GREEN	al_map_rgb(0,255,0)
#define RED		al_map_rgb(255,0,0)
#define BLUE	al_map_rgb(0,0,255)


namespace LSW {
	namespace v4 {

		template <typename T>
		constexpr auto operator+(T e) noexcept
			-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
		{
			return static_cast<std::underlying_type_t<T>>(e);
		}

	}
}
