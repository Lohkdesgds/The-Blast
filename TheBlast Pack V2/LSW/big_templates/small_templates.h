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

		namespace Constants {

			template<typename H> const auto lambda_null_load = [](const char* p, H*& r) -> bool { return false; };
			template<typename H> const auto lambda_null_unload = [](H*& b) -> void { return; };

			template<typename K> const auto lambda_default_load = [](const char* p, K*& r) -> bool { return (r = new K()); };
			template<typename K> const auto lambda_default_unload = [](K*& b) -> void { if (b) delete b; b = nullptr; };
		}

	}
}
