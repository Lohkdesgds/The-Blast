#pragma once


#include "..\..\all\enable_all.h"
//#include "..\_external\dirent.h"

namespace LSW {
	namespace v2 {
		namespace Stacker {

			const size_t stack_size_default = 4096;

			struct _stack_assist {
				std::string filename;
				__int64 siz_o_file = 0;
			};

			class _stack_f {
			protected:
				std::vector<_stack_assist> stack;
				std::string orig;
				float perc = 0.0;
			public:
				const size_t size();
				const float getPerc();
			};

			class extractor : public _stack_f
			{
			public:
				const bool interpret(const std::string);
				const bool extractAll(std::string = "");
			};

			class compactor : public _stack_f
			{
			public:
				void insert(std::string);
				void insertF(std::string);
				void setOut(const std::string);
				void compactAll();
			};

			void _createPath(std::string);
			__int64 _getFileSize(const std::string&);
			void _folderSearch(std::list<std::string>&, const std::string);
		}
	}
}

