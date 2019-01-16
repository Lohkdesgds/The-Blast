#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "..\..\_external\dirent.h"

namespace LSW {
	namespace v2 {
		namespace Stacker {

			struct _stack_assist {
				std::string filename;
				__int64 siz_o_file = 0;
			};

			class _stack_f {
			protected:
				std::vector<_stack_assist> stack;
				std::string orig;
			};

			class extractor : public _stack_f
			{
			public:
				const bool interpret(const std::string);
				const bool extractAll();
			};

			class compactor : public _stack_f
			{
			public:
				void insert(std::string);
				void setOut(const std::string);
				void compactAll();
			};

			void _createPath(std::string);
			__int64 _getFileSize(const std::string&);
		}
	}
}