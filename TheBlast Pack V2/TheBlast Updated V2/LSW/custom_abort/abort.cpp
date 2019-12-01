#include "abort.h"

namespace LSW {
	namespace v4 {
		namespace Abort {

			abort::abort(std::string a, std::string b, std::string c, const int d)
			{
				func = a;
				from_where = b;
				ddetails = c;
				errn = d;
			}
			std::string LSW::v4::Abort::abort::from()
			{
				return from_where;
			}

			std::string abort::function()
			{
				return func;
			}

			std::string abort::details()
			{
				return ddetails;
			}

			int abort::getErrN()
			{
				return errn;
			}


		}
	}
}