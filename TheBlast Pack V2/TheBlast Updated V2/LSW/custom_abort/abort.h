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
			public:
				abort(std::string, std::string, std::string = ""); // what's wrong, where, and an extended explanation
				std::string from();
				std::string function();
				std::string details();
			};
		}
	}
}