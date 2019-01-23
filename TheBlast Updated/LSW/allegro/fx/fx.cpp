#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Fx {
			bubbles::bubbles()
			{
				firstcall = true;
			}
			bubbles::bubbles(const unsigned amout, const float mfps, const int layer)
			{
				init(amout, mfps, layer);
			}
			bubbles::~bubbles()
			{
				positions.clear();
				disguy->remove("BKG_NULL");
				//imglw->unload();

				d_images_database img_data;
				d_sprite_database spr_data;
				img_data.remove("BKG_NULL");
				spr_data.remove("BKG_NULL");
				delete disguy;
				imglw->unload();
				delete imglw;
				disguy = nullptr;
				imglw = nullptr;
			}

			void bubbles::init(const unsigned amout, const float mfps, const int layer)
			{
				if (firstcall) {

					d_images_database img_data;
					d_sprite_database spr_data;
					ALLEGRO_BITMAP* bp;

					img_data.create(imglw);
					imglw->create(-1, -1);
					imglw->setID("BKG_NULL");
					imglw->get(bp);
					siz[0] = al_get_bitmap_width(bp);
					siz[1] = al_get_bitmap_height(bp);

					spr_data.create(disguy);
					disguy->add("BKG_NULL");
					disguy->setID("BKG_NULL");
					disguy->set(Sprite::LAYER, layer);
					disguy->set(Sprite::SCALEG, 2.0);
					disguy->set(Sprite::AFFECTED_BY_CAM, false);

					if (mfps > 1.0) fps = mfps;
					lastdraw = al_get_time();

					if (amout > 0) {
						amountofentities = amout;
					}

					particle poss;

					if (positions.size() > 0) positions.clear();

					for (unsigned c = 0; c < amountofentities; c++)
					{
						poss.posx = 1.0 - (rand() % 1000) / 500.0;
						poss.posy = 1.0 - (rand() % 1000) / 500.0;

						positions.push_back(poss);
					}
					firstcall = false;
				}
			}

			void bubbles::draw()
			{
				if (firstcall) {
					throw "BUBBLES::DRAW NOT STARTED!";
					return;
				}
				/*if (updatebitmap) {
					if (lastbitmap) al_destroy_bitmap(lastbitmap);

					ALLEGRO_DISPLAY* dis = al_get_current_display();
					assert(dis);

					if (!(lastbitmap = al_create_bitmap(al_get_display_width(dis), al_get_display_height(dis)))) lastbitmap = nullptr;
					else {
						lastbitmapwidth = al_get_display_width(dis);
						updatebitmap = false;
					}
				}*/

				

				//al_draw_filled_rectangle(0, 0, al_get_bitmap_width(lastbitmap), al_get_bitmap_height(lastbitmap), al_map_rgba_f(0.0, 0.0, 0.0, (1.0 - (amountofblur_bubbles / 100.0))*0.1)); 
				if (alreadyreset) {
					alreadyreset = false;

					ALLEGRO_BITMAP* lastpoint = al_get_target_bitmap();
					ALLEGRO_BITMAP* dis;
					imglw->get(dis);

					al_set_target_bitmap(dis);

					for (auto& i : positions)
					{
						al_draw_filled_circle(i.lastpositionscalculated[0], i.lastpositionscalculated[1], i.lastsize, al_map_rgba_f(maxone(al_get_time()*0.3*((rand() % 1000) / 1000.0))*(1.0 - (Defaults::amountofblur_bubbles / 100.0)), maxone(al_get_time()*0.1*((rand() % 1000) / 1000.0))*(1.0 - (Defaults::amountofblur_bubbles / 100.0)), maxone(al_get_time()*0.8*((rand() % 1000) / 1000.0))*(1.0 - (Defaults::amountofblur_bubbles / 100.0)), (1.0 - (Defaults::amountofblur_bubbles / 100.0))));
					}

					al_set_target_bitmap(lastpoint);
				}

				//al_draw_bitmap(dis, -1.0, -1.0, 0);
				
			}
			void bubbles::think()
			{
				if (firstcall) {
					throw "BUBBLES::DRAW NOT STARTED!";
					return;
				}
				//updatebitmap = (lastbitmapwidth != nec.getINT("Display", "Width"));

				if ((al_get_time() - lastdraw) >= (1.0 / fps))
				{
					lastdraw = al_get_time();
					alreadyreset = true;

					ALLEGRO_BITMAP* bp;
					imglw->get(bp);

					siz[0] = al_get_bitmap_width(bp);
					siz[1] = al_get_bitmap_height(bp);

					//gotten = true;
				
					for (auto& i : positions)
					{
						i.lastsize = (12.0 + (rand() % 500) / 100.0)*3.0 * sqrt(siz[0]*siz[1])/1440.0;
						i.posx = 1.0 - (rand() % 1000) / 500.0;
						i.posy = 1.0 - (rand() % 1000) / 500.0;

						if (i.posx > 1.0) i.posx = 1.0;
						if (i.posy > 1.0) i.posy = 1.0;
						if (i.posx < -1.0) i.posx = -1.0;
						if (i.posy < -1.0) i.posy = -1.0;

						i.lastpositionscalculated[0] = ((i.posx + 1.0) / 2.0) * siz[0];
						i.lastpositionscalculated[1] = ((i.posy + 1.0) / 2.0) * siz[1];
					}
				}
			}

			const double maxone(double gotten, const double prop)
			{
				while (gotten > 1.0) {
					if (gotten > 1000.0) gotten -= 1000.0;
					else if (gotten > 100000.0) gotten -= 100000.0;
					else if (gotten > 2.0) gotten -= 1.0;
					else {
						if (gotten > 1.0) gotten = 2.0 - gotten;
						else gotten -= 1.0;
					}
				}
				return gotten * prop;
			}
			lines::lines(const int layer)
			{
				d_images_database img_data;
				d_sprite_database spr_data;
				ALLEGRO_BITMAP* bp;

				img_data.create(imglw);
				imglw->create(-1, -1);
				imglw->setID("LINES_NULL");
				imglw->get(bp);

				rad = ((al_get_bitmap_width(bp) > al_get_bitmap_height(bp)) ? al_get_bitmap_width(bp) : al_get_bitmap_height(bp));

				spr_data.create(disguy);
				disguy->add("LINES_NULL");
				disguy->setID("LINES_NULL");
				disguy->set(Sprite::LAYER, layer);
				disguy->set(Sprite::SCALEG, 2.0);
				disguy->set(Sprite::AFFECTED_BY_CAM, false);
			}
			lines::~lines()
			{
				disguy->remove("LINES_NULL");
				//imglw->unload();

				d_images_database img_data;
				d_sprite_database spr_data;
				img_data.remove("LINES_NULL");
				spr_data.remove("LINES_NULL");
				delete disguy;
				imglw->unload();
				delete imglw;
				disguy = nullptr;
				imglw = nullptr;
			}
			void lines::draw()
			{
				ALLEGRO_BITMAP* g = al_get_target_bitmap();
				assert(g);

				ALLEGRO_BITMAP* bp;
				imglw->get(bp);
				al_set_target_bitmap(bp);

				al_clear_to_color(al_map_rgb_f(maxone(0.5 + cos(al_get_time()*1.75), 0.6), maxone(0.5 + cos(al_get_time()*2.15), 0.6), maxone((0.5 + cos(al_get_time()*1.11)), 0.6)));

				//int rad = 2;
				int xpress = al_get_bitmap_width(bp);
				int ypress = al_get_bitmap_height(bp);

				unsigned counter = 0;

				for (double radius = 0; radius < ALLEGRO_PI; radius += Defaults::precision_rotation_draw)
				{
					counter++;
					if (counter < Defaults::amountof_linesperline_draw) {
						double posx = rad * cos(radius + al_get_time() / 5.0);
						double posy = rad * sin(radius + al_get_time() / 5.0);

						al_draw_line((xpress / 2) + posx, (ypress / 2) + posy, (xpress / 2) - posx, (ypress / 2) - posy, al_map_rgb_f(maxone(0.5 + cos(al_get_time()*0.52), 0.7), maxone(0.5 + cos(al_get_time()*0.15), 0.7), maxone((0.5 + cos(al_get_time()*0.54)), 0.7)), 5.0);
					}
					else if (counter >= Defaults::amountof_linesperline_draw * 2) {
						counter = 0;
					}
				}
				al_set_target_bitmap(g);
			}
}
	}
}