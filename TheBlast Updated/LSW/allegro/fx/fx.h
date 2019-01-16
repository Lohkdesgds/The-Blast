#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Fx {
			struct particle {
				double posx, posy;
				double lastsize = 1.0;
				double lastpositionscalculated[2];
			};
			
			class bubbles {
				std::vector<particle> positions;
				unsigned amountofentities = 2;
				float fps = 30;
				double lastdraw = 0;
				bool alreadyreset = false;

				bool firstcall = true;

				//ALLEGRO_BITMAP* lastbitmap = nullptr;
				Image::image_low* imglw = nullptr;
				Sprite::sprite* disguy = nullptr;

				int siz[2] = { 0, 0 };
				//int lastbitmapwidth = 0;
				//bool updatebitmap = false;
			public:
				// amount of entities running forever :3 and fps
				bubbles(const unsigned, const float, const int = 0);
				~bubbles();

				void draw();
				void think();
			};

			const double maxone(double);

			class lines {
				Image::image_low* imglw = nullptr;
				Sprite::sprite* disguy = nullptr;
				int rad = 2;
			public:
				lines(const int = 0);
				~lines();

				void draw();
			};
		}
	}
}