#pragma once

// STANDALONE (ALONE)

#include <string>

namespace LSW {
	namespace v4 {
		namespace Abort {

			class abort {
				std::string from_where;
				std::string dtls;
				int errn = 0;
			public:
				abort(std::string, std::string = "", const int = 0); // __FUNCSIG__ and what
				std::string from();
				std::string details();
				int getErrN();
			};
		}
	}
}