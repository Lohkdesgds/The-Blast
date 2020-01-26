#include "abort.h"

namespace LSW {
	namespace v4 {
		namespace Abort {
			abort::abort(std::string from, std::string detail, const int d)
			{
				from_where = from;
				dtls = detail;
				errn = d;
			}
			std::string LSW::v4::Abort::abort::from()
			{
				return from_where;
			}
			std::string abort::details()
			{
				return dtls;
			}

			int abort::getErrN()
			{
				return errn;
			}
		}
	}
}