#include "menutton.h"

namespace LSW {
	namespace v4 {
		size_t Button::count = 0;
		
		Constants::io__sprite_tie_func_to_state castTieToSprite(const Constants::io__buttons_shared_tie_func v) {
			switch (v) {
			case Constants::io__buttons_shared_tie_func::MOUSE_ON:
				return Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON;
			case Constants::io__buttons_shared_tie_func::MOUSE_CLICK:
				return Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK;
			case Constants::io__buttons_shared_tie_func::MOUSE_UNCLICK:
				return Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_UNCLICK;
			case Constants::io__buttons_shared_tie_func::NO_COLLISION:
				return Constants::io__sprite_tie_func_to_state::COLLISION_NONE;
			case Constants::io__buttons_shared_tie_func::ON_UPDATE:
				return Constants::io__sprite_tie_func_to_state::WHEN_BITMAPS_RESIZED_AUTO;
			case Constants::io__buttons_shared_tie_func::ON_DRAW:
				return Constants::io__sprite_tie_func_to_state::ON_DRAW;
			}
			return Constants::io__sprite_tie_func_to_state();
		}
		Constants::io__text_tie_func_to_state   castTieToText(const Constants::io__buttons_shared_tie_func v) {
			switch (v) {
			case Constants::io__buttons_shared_tie_func::MOUSE_ON:
				return Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON;
			case Constants::io__buttons_shared_tie_func::MOUSE_CLICK:
				return Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK;
			case Constants::io__buttons_shared_tie_func::MOUSE_UNCLICK:
				return Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_UNCLICK;
			case Constants::io__buttons_shared_tie_func::NO_COLLISION:
				return Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE;
			case Constants::io__buttons_shared_tie_func::ON_UPDATE:
				return Constants::io__text_tie_func_to_state::ON_UPDATE;
			case Constants::io__buttons_shared_tie_func::ON_DRAW:
				return Constants::io__text_tie_func_to_state::ON_DRAW;
			}
			return Constants::io__text_tie_func_to_state();
		}
		bool isThisNotDefaultColor(const ALLEGRO_COLOR c) {
			return !((c.a == c.b) && (c.b == c.g) && (c.g == c.r) && (c.r == 1.0));
		}


		void Button::init(std::string id)
		{
			if (!spr) {

				if (id.length() == 0) {
					id = "__I_BUTTON_AUTOGEN_" + std::to_string(count++) + "_R" + std::to_string(rand() % 1000);
				}
				
				ResourceOf<Sprite> sprites;
				ResourceOf<Text> texts;

				if (!(spr = sprites.create(id))) throw Abort::warn(__FUNCSIG__, "Cannot call Sprites to load/get " + id);
				if (!(txt = texts.create(id))) throw Abort::warn(__FUNCSIG__, "Cannot call Texts to load/get " + id);

				id_based_from = id;

				spr->set(Constants::io__sprite_string::ID, id);
				spr->set(Constants::io__sprite_boolean::DRAW, true);
				spr->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
				spr->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				spr->set(Constants::io__sprite_double::SCALEG, Constants::buttons_sprite_scaleg_def);
				spr->set(Constants::io__sprite_double::SCALEX, Constants::buttons_sprite_scalex_def);
				spr->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
				setColor(Constants::io__buttons_color_shared::ONLY_SPRITE, Constants::buttons__butc_def);

				txt->set(Constants::io__text_string::ID, id + std::string("_T"));
				txt->set(Constants::io__text_string::FOLLOW_SPRITE, id);
				txt->set(Constants::io__text_string::FONT, "DEFAULT");
				txt->set(Constants::io__text_boolean::SHOW, true);
				txt->set(Constants::io__text_string::STRING, " ");
				txt->set(Constants::io__text_double::SCALEG, Constants::buttons_text___scaleg_def);
				txt->set(Constants::io__text_double::SCALEX, Constants::buttons_text___scalex_def);
				txt->set(Constants::io__text_double::POSY, -0.055);
				txt->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
				setColor(Constants::io__buttons_color_shared::ONLY_TEXT, Constants::buttons__txtc_def);

				for (int i = +Constants::io__buttons_shared_tie_func::MOUSE_ON; i < +Constants::io__buttons_shared_tie_func::size; i++) {
					Constants::io__buttons_shared_tie_func e = (Constants::io__buttons_shared_tie_func)i;

					auto spr_c = castTieToSprite(e);
					auto txt_c = castTieToText(e);

					// they will call the same thing, but the same thing works kinda differently to each
					spr->hook(spr_c, [&, e]() {
						if (hooks[+e]) {
							nullstr.clear();

							dism.lock();
							hooks[+e](spr, (Text*)nullptr, nullstr);
							dism.unlock();
						}}, true);
					txt->hook(txt_c, [&, e](std::string& k) {
						if (hooks[+e]) {
							dism.lock();
							hooks[+e]((Sprite*)nullptr, txt, k);
							dism.unlock();
						}});
				}
			}
		}

		Button::Button()
		{
			init("");
		}

		Button::Button(const char* id)
		{
			init(id);
		}
		Button::Button(std::string& id)
		{
			init(id);
			id = id_based_from;
		}
		Sprite& Button::getSprite()
		{
			return *spr;
		}
		Text& Button::getText()
		{
			return *txt;
		}
		void Button::setText(const std::string t)
		{
			txt->set(Constants::io__text_string::STRING, t);
		}
		void Button::setLayers(std::initializer_list<int> layers)
		{
			bool frst = true;
			for (auto i : layers) {
				if (frst){
					spr->set(Constants::io__sprite_integer::LAYER, i);
					txt->set(Constants::io__text_integer::LAYER, i);
					frst = false;
				}
				else {
					spr->set(Constants::io__sprite_integer::ADD_ALT_LAYER, i);
					txt->set(Constants::io__text_integer::ADD_ALT_LAYER, i);
				}
			}
		}
		void Button::removeLayers(std::initializer_list<int> layers)
		{
			for (auto i : layers) {
				spr->set(Constants::io__sprite_integer::REMOVE_ALT_LAYER, i);
				txt->set(Constants::io__text_integer::REMOVE_ALT_LAYER, i);
			}
		}
		void Button::setColor(const Constants::io__buttons_color_shared e, const ALLEGRO_COLOR v)
		{
			switch (e) {
			case Constants::io__buttons_color_shared::ONLY_SPRITE:
				spr->set(Constants::io__sprite_color::TINT, v);
				txt->set(Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD, false);
				break;
			case Constants::io__buttons_color_shared::ONLY_TEXT:
				txt->set(Constants::io__text_color::COLOR, v);
				txt->set(Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD, false);
				break;
			case Constants::io__buttons_color_shared::SET_BOTH:
				spr->set(Constants::io__sprite_color::TINT, v);
				txt->set(Constants::io__text_color::COLOR, v);
				txt->set(Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD, false);
				break;
			case Constants::io__buttons_color_shared::SET_SPRITE_AND_TEXT_FOLLOW:
				spr->set(Constants::io__sprite_color::TINT, v);
				txt->set(Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD, true);
				break;
			case Constants::io__buttons_color_shared::SET_TEXT_SHADOW:
				txt->set(Constants::io__text_color::SHADOW_COLOR, v);
				if (isThisNotDefaultColor(v)) {
					txt->set(Constants::io__text_double::SHADOW_DIST_X, Constants::buttons_text___shadwx_def);
					txt->set(Constants::io__text_double::SHADOW_DIST_Y, Constants::buttons_text___shadwy_def);
				}
				break;
			}
		}
		void Button::setPosX(const double x)
		{
			spr->set(Constants::io__sprite_double::POSX, x);
		}
		void Button::setPosY(const double y)
		{
			spr->set(Constants::io__sprite_double::POSY, y);
		}
		void Button::setPos(const double x, const double y)
		{
			setPosX(x);
			setPosY(y);
		}
		void Button::setPropBasedScale(const double a, const double b)
		{
			spr->set(Constants::io__sprite_double::SCALEG, a * Constants::buttons_sprite_scaleg_def);
			spr->set(Constants::io__sprite_double::SCALEX, b * Constants::buttons_sprite_scalex_def);
		}
		void Button::setLinks(std::initializer_list<__combo_0> stat)
		{
			for (auto& i : stat) {
				spr->set(i.a, i.b);
			}
		}
		void Button::addBitmaps(bool use_state_as_bmp, std::initializer_list<const char*> bitmaps)
		{
			spr->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, use_state_as_bmp);
			for (auto i : bitmaps) {
				spr->set(Constants::io__sprite_string::ADD, i);
			}
		}
		void Button::delBitmaps(std::initializer_list<const char*> bitmaps)
		{
			for (auto i : bitmaps) {
				spr->set(Constants::io__sprite_string::REMOVE, i);
			}
		}
		void Button::hook(const Constants::io__buttons_shared_tie_func v, const std::function<void(Sprite*, Text*, std::string&)> f)
		{
			if (v != Constants::io__buttons_shared_tie_func::size) hooks[+v] = f;
		}
		void Button::unhook(const Constants::io__buttons_shared_tie_func v)
		{
			hook(v, std::function<void(Sprite*, Text*, std::string&)>());
		}
		void SliderX::initS()
		{
			if (!spr_lr) {
				ResourceOf<Sprite> sprites;

				if (!(spr_lr = sprites.create(id_based_from + "_OVER"))) throw Abort::warn(__FUNCSIG__, "Cannot call Sprites to load/get " + id_based_from + "_OVER");


				/// >>>>>>>>>>>>>>>>>>>>>>>>>>| INJECTION TO BUTTON |<<<<<<<<<<<<<<<<<<<<<<<<<< ///
				perma_spr_lr_sec = [&]() { // set spr_lr position based on spr
					double posx, posy;
					spr->get(Constants::io__sprite_double::POSY, posy);
					spr->get(Constants::io__sprite_double::POSX, posx);
					double finala = (last_delta - 0.5) * 2.0; // -1 to 1
					finala += posx; // adjust to posx
					finala *= getHalfMinMax(); // 0.342
					// FOLLOW
					spr_lr->set(Constants::io__sprite_double::POSY, posy);
					spr_lr->set(Constants::io__sprite_double::POSX, finala);
				};
				// inject
				spr_lr->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [&]() { perma_spr_lr_sec(); });
				/// >>>>>>>>>>>>>>>>>>>>>>>>>>| INJECTION TO BUTTON |<<<<<<<<<<<<<<<<<<<<<<<<<< ///

				setColor(Constants::io__buttons_color_shared::ONLY_SPRITE, Constants::buttons__btcl_def); // lower must be darker lmao

				spr_lr->set(Constants::io__sprite_string::ID, id_based_from + "_OVER");
				spr_lr->set(Constants::io__sprite_boolean::DRAW, true);
				spr_lr->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
				spr_lr->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				spr_lr->set(Constants::io__sprite_double::SCALEG, Constants::buttons_sprite_scaleg_def);
				spr_lr->set(Constants::io__sprite_double::SCALEX, Constants::buttons_sprite_scalsx_def);
				spr_lr->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
				spr_lr->set(Constants::io__sprite_double::POSY, 0.0);
				spr_lr->set(Constants::io__sprite_double::POSX, 0.0);
				spr_lr->set(Constants::io__sprite_color::TINT, Constants::buttons__butc_def);

				for (int i = +Constants::io__buttons_shared_tie_func::MOUSE_ON; i < +Constants::io__buttons_shared_tie_func::size; i++) {
					Constants::io__buttons_shared_tie_func e = (Constants::io__buttons_shared_tie_func)i;

					auto spr_c = castTieToSprite(e);
					auto txt_c = castTieToText(e);

					// like the Button, but different
					spr->hook(spr_c, [&, e]() {if (hook_slider[+e]) { // max dx in each dir = ((big_x - slider_x) / 2) * scale
						if (hook_slider[+e]) {
							double max_lat = getHalfMinMax();
							double off_x;
							spr->get(Constants::io__sprite_double::POSX, off_x);

							Database conf;
							double dx = 0;
							conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
							dx -= off_x;
							if (dx > max_lat) dx = max_lat;
							if (dx < -max_lat) dx = -max_lat;
							double vol = (dx + max_lat) / (max_lat * 2.0);
							if (vol > 1.0) vol = 1.0;
							if (vol < 0.0) vol = 0.0;
							nullstr.clear();

							dism.lock();
							hook_slider[+e](vol, spr_lr, spr, txt, nullstr); // if want to set spr_lr pos, setPercPos(vol);
							dism.unlock();
						}
					} }, true);
					txt->hook(txt_c, [&, e](std::string& k) {
						if (hook_slider[+e]) {
							double max_lat = getHalfMinMax();
							double off_x;
							spr->get(Constants::io__sprite_double::POSX, off_x);

							Database conf;
							double dx = 0;
							conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
							dx -= off_x;
							if (dx > max_lat) dx = max_lat;
							if (dx < -max_lat) dx = -max_lat;
							double vol = (dx + max_lat) / (max_lat * 2.0);
							if (vol > 1.0) vol = 1.0;
							if (vol < 0.0) vol = 0.0;

							dism.lock();
							hook_slider[+e](vol, spr_lr, spr, txt, k); // if want to set spr_lr pos, setPercPos(vol);
							dism.unlock();
						}
					});
				}
			}
		}
		SliderX::SliderX() : Button()
		{
			initS();
		}
		SliderX::SliderX(const char* id) : Button(id)
		{
			initS();
		}
		SliderX::SliderX(std::string& id) : Button(id)
		{
			initS();
		}
		Sprite& SliderX::getOverSprite()
		{
			return *spr_lr;
		}
		void SliderX::setLayers(std::initializer_list<int> layers)
		{
			Button::setLayers(layers);
			bool frst = true;
			for (auto i : layers) {
				if (frst) {
					spr_lr->set(Constants::io__sprite_integer::LAYER, i);
					frst = false;
				}
				else {
					spr_lr->set(Constants::io__sprite_integer::ADD_ALT_LAYER, i);
				}
			}
		}
		void SliderX::removeLayers(std::initializer_list<int> layers)
		{
			Button::removeLayers(layers);
			for (auto i : layers) {
				spr_lr->set(Constants::io__sprite_integer::REMOVE_ALT_LAYER, i);
			}
		}
		void SliderX::setPosX(const double x)
		{
			Button::setPosX(x);
			spr_lr->set(Constants::io__sprite_double::POSX, x);
		}
		void SliderX::setPosY(const double y)
		{
			Button::setPosY(y);
			spr_lr->set(Constants::io__sprite_double::POSY, y);
		}
		void SliderX::setPos(const double x, const double y)
		{
			setPosX(x);
			setPosY(y);
		}
		void SliderX::setPropBasedScale(const double g, const double x)
		{
			Button::setPropBasedScale(g, x);
			spr_lr->set(Constants::io__sprite_double::SCALEG, g * Constants::buttons_sprite_scaleg_def);
			spr_lr->set(Constants::io__sprite_double::SCALEX, x * Constants::buttons_sprite_scalex_def);
		}
		void SliderX::setPropSizeXSlider(const double p)
		{
			if (p < 0.0 || p > 1.0) return;
			double sg;
			spr->get(Constants::io__sprite_double::SCALEX, sg);
			spr_lr->set(Constants::io__sprite_double::SCALEX, sg * p);
		}
		void SliderX::setPercPos(const double perc)
		{
			if (perc < 0.0) last_delta = 0.0;
			else if (perc > 1.0) last_delta = 1.0;
			else last_delta = perc;
		}
		double SliderX::getHalfMinMax()
		{
			double big_x, scale_corr, sizx;
			spr->get(Constants::io__sprite_double::SCALEX, big_x);
			spr_lr->get(Constants::io__sprite_double::SCALEG, scale_corr);
			spr_lr->get(Constants::io__sprite_double::SCALEX, sizx);
			return scale_corr * (big_x - sizx) * 0.5;
		}
		void SliderX::setLinks(std::initializer_list<__combo_0> stat)
		{
			Button::setLinks(stat);
			for (auto& i : stat) {
				spr_lr->set(i.a, i.b);
			}
		}
		void SliderX::addBitmaps(bool use_state_as_bmp, std::initializer_list<const char*> bitmaps)
		{
			Button::addBitmaps(use_state_as_bmp, bitmaps);
			spr_lr->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, use_state_as_bmp);
			for (auto i : bitmaps) {
				spr_lr->set(Constants::io__sprite_string::ADD, i);
			}
		}
		void SliderX::delBitmaps(std::initializer_list<const char*> bitmaps)
		{
			Button::delBitmaps(bitmaps);
			for (auto i : bitmaps) {
				spr_lr->set(Constants::io__sprite_string::REMOVE, i);
			}
		}
		void SliderX::hook(const Constants::io__buttons_shared_tie_func v, const std::function<void(const double,Sprite*,Sprite*,Text*,std::string&)> f)
		{
			if (v != Constants::io__buttons_shared_tie_func::size) hook_slider[+v] = f;
		}
		void SliderX::unhook(const Constants::io__buttons_shared_tie_func v)
		{
			hook(v, std::function<void(const double,Sprite*,Sprite*,Text*,std::string&)>());
		}
}
}