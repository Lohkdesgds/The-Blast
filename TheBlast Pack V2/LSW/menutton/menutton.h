#pragma once

#include "..\drawing\drawing.h"
#include "..\big_templates\big_templates.h"

/*
Objective:
- A layer on the library so common stuff like buttons can be easily done
*/

namespace LSW {
	namespace v4 {
		
		namespace Constants {

			const double buttons_sprite_scaleg_def = 0.16,
						 buttons_text___scaleg_def = 0.08,
						 buttons_sprite_scalex_def = 5.5,
						 buttons_text___scalex_def = 0.7,
						 buttons_sprite_scalsx_def = 1.2, // slider
						 buttons_text___shadwx_def = 0.0020,
						 buttons_text___shadwy_def = 0.0025;
			const ALLEGRO_COLOR buttons__txtc_def = al_map_rgba_f(0.71, 1.00, 1.00, 1.00),
								buttons__butc_def = al_map_rgba_f(0.05, 1.00, 0.45, 1.00),
								buttons__btcl_def = al_map_rgba_f(0.05, 1.00, 0.45, 1.00); // if slider

			// TEXT based: SPRITE_COLLISION_MOUSE_ON, SPRITE_COLLISION_MOUSE_CLICK, SPRITE_COLLISION_MOUSE_UNCLICK, SPRITE_COLLISION_NONE, ON_UPDATE, ON_DRAW
			// SPRITE based: COLLISION_MOUSE_ON, COLLISION_MOUSE_CLICK, COLLISION_MOUSE_UNCLICK, COLLISION_NONE, WHEN_BITMAPS_RESIZED_AUTO, ON_DRAW
			enum class io__buttons_shared_tie_func{MOUSE_ON, MOUSE_CLICK, MOUSE_UNCLICK, NO_COLLISION, ON_UPDATE, ON_DRAW, size };

			enum class io__buttons_color_shared{ONLY_SPRITE,ONLY_TEXT,SET_BOTH,SET_SPRITE_AND_TEXT_FOLLOW,SET_TEXT_SHADOW};
		}

		Constants::io__sprite_tie_func_to_state castTieToSprite(const Constants::io__buttons_shared_tie_func);
		Constants::io__text_tie_func_to_state   castTieToText(const Constants::io__buttons_shared_tie_func);
		bool isThisNotDefaultColor(const ALLEGRO_COLOR);

		class Button {
			static size_t count;

		protected:
			Sprite* spr = nullptr;
			Text* txt = nullptr;
			std::mutex dism;

			std::string nullstr;
			std::function<void(Sprite*, Text*, std::string&)> hooks[+Constants::io__buttons_shared_tie_func::size] = { std::function<void(Sprite*, Text*, std::string&)>() };
			std::string id_based_from;

			struct __combo_0 {
				Constants::io__sprite_tie_func_to_state a;
				size_t b;
			};

			void init(std::string);
		public:
			Button();
			Button(const char*);
			Button(std::string&); // get back ID

			Sprite& getSprite();
			Text& getText();

			void setText(const std::string);

			void setLayers(std::initializer_list<int>); // one = the layer, more than one = the layer + add_alt_layer
			void removeLayers(std::initializer_list<int>); // removes only added by add_alt_layer

			void setColor(const Constants::io__buttons_color_shared, const ALLEGRO_COLOR = al_map_rgb(255,255,255));

			void setPosX(const double);
			void setPosY(const double);
			void setPos(const double, const double);
			void setPropBasedScale(const double, const double = 1.0); // G, X

			void setLinks(std::initializer_list<__combo_0>); // addBitmaps with true agr enables Link (aka state to bitmap). else no difference

			void addBitmaps(bool, std::initializer_list<const char*>); // use state as bitmap
			void delBitmaps(std::initializer_list<const char*>);

			void hook(const Constants::io__buttons_shared_tie_func, const std::function<void(Sprite*, Text*, std::string&)>); // one function, two arguments. When Sprite triggers, text will be NULL; if Text, Sprite will be NULL!
			void unhook(const Constants::io__buttons_shared_tie_func);
		};


		class SliderX : public Button {
			Sprite* spr_lr = nullptr;
			double last_delta = 0.5;

			std::function<void(void)> perma_spr_lr_sec = std::function<void(void)>();

			std::function<void(const double,Sprite*,Sprite*,Text*,std::string&)> hook_slider[+Constants::io__buttons_shared_tie_func::size] = { std::function<void(const double,Sprite*,Sprite*,Text*,std::string&)>() };

			void initS();
		public:
			SliderX();
			SliderX(const char*);
			SliderX(std::string&);

			Sprite& getOverSprite();

			void setLayers(std::initializer_list<int>); // one = the layer, more than one = the layer + add_alt_layer
			void removeLayers(std::initializer_list<int>); // removes only added by add_alt_layer

			void setPosX(const double);
			void setPosY(const double);
			void setPos(const double, const double);
			void setPropBasedScale(const double, const double = 1.0); // G, X

			void setPropSizeXSlider(const double = 0.22); // scale based on Sprite scale (horiz)

			void setPercPos(const double = 1.0); // 0..1, value
			double getHalfMinMax(); // based on sprites, what are the limits?

			void setLinks(std::initializer_list<__combo_0>); // addBitmaps with true agr enables Link (aka state to bitmap). else no difference

			void addBitmaps(bool, std::initializer_list<const char*>); // use state as bitmap
			void delBitmaps(std::initializer_list<const char*>);

			void hook(const Constants::io__buttons_shared_tie_func, const std::function<void(const double,Sprite*,Sprite*,Text*,std::string&)>); // the double it receives is the position %% (0 to 1) of the slider, the first is lower, second button, text 3rd
			void unhook(const Constants::io__buttons_shared_tie_func);
		};

		

		// big templates thing
		template <> __template_static_vector<Button>::_i<Button> __template_static_vector<Button>::data = {
			[](std::string& p, Button*& b) -> bool {
				if (p.length() > 0) return (b = new Button(p.c_str())); // const char* can't be modified
				return (b = new Button(p)); // p can be modified
			}
		};
		template <> __template_static_vector<SliderX>::_i<SliderX> __template_static_vector<SliderX>::data = {
			[](std::string& p, SliderX*& b) -> bool {
				if (p.length() > 0) return (b = new SliderX(p.c_str())); // const char* can't be modified
				return (b = new SliderX(p)); // p can be modified
			}
		};
	}
}