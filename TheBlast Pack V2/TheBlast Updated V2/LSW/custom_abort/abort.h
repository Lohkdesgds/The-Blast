#pragma once

// STANDALONE (ALONE)

#include <string>

namespace LSW {
	namespace v4 {
		namespace Abort {

			class abort {
				std::string func;
				std::string from_where;
				std::string ddetails;
				int errn = 0;
			public:
				abort(std::string, std::string, std::string = "", const int = 0); // what's wrong, where, an extended explanation, internal debug resolving (try catch dealing with the throw)
				std::string from();
				std::string function();
				std::string details();
				int getErrN();
			};
		}
	}
}