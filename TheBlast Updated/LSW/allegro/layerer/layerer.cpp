#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Layer {

			_all_layers layerer::layers;

			each_layer& layerer::getOne(const int o)
			{
				return this->layers.l[o];
			}
			each_layer& layerer::getNow()
			{
				return this->layers.l[layers.being_used];
			}
			void layerer::setUsingNow(const int o)
			{
				layers.being_used = o;
			}
			void layerer::clearNumber(const int o)
			{
				layers.l.erase(o);
			}

			_layer_down & each_layer::set(const int o)
			{
				/*if (lr[o].colliding.size() == 0) {
					lr[o].colliding[o] = true;
				}*/
				return lr[o];
			}

			void each_layer::unset(const int o)
			{
				lr.erase(o);
			}

			void each_layer::setMode(const mode m)
			{
				moe = m;
			}

			const mode each_layer::getMode()
			{
				return moe;
			}

			void each_layer::save_package(void * p)
			{
				ppp = p;
			}

			void * each_layer::get_package()
			{
				return ppp;
			}

			std::map<int, _layer_down>& each_layer::work()
			{
				return lr;
			}

		}
	}
}