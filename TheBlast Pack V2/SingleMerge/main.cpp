#include <iostream>
#include <string>
#include <Windows.h>

#include "..\LSW\hash\sha256.h"

using namespace LSW::v4;

const std::string seaching_for = "0000000000000000000000000000000000000000000000000000000000000000";

void pause() {
	int a;
	std::cin >> a;
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout << "Please drop the .exe file with the data.zip pack so I can merge them." << std::endl;
		Sleep(10000);
		return 0;
	}

	FILE *a, *b;
	std::string a_s = argv[1], b_s = argv[2];

	if (a_s.find(".zip") != std::string::npos) {
		a_s = argv[2];
		b_s = argv[1];
	}

	auto err0 = fopen_s(&a, a_s.c_str(), "ab+"); // exe
	auto err1 = fopen_s(&b, b_s.c_str(), "rb"); // zip
	std::string defu = "\n%LSW_DATA\n";

	if (err0 == 0 && err1 == 0 && a && b) {
		char buf[512];
		bool still = true;

		fwrite(defu.c_str(), sizeof(char), defu.length(), a);

		while (still) {
			auto readd = fread_s(buf, sizeof(buf), sizeof(char), 512, b);
			fwrite(buf, sizeof(char), readd, a);

			if (readd != 512) still = false;
		}
		if (b) fclose(b);

		std::cout << "Installed successfully! Generating SHA256..." << std::endl;

		fseek(a, 0, SEEK_SET);
		{
			std::string content;
			
			while (!feof(a)) {
				char ubuf;
				fread_s(&ubuf, 1, sizeof(char), 1, a);
				content += ubuf;
			}

			std::string sha = imported::sha256(content);

			printf_s("Your SHA256 code is: %s", sha.c_str());

			if (a) fclose(a);

			pause();
		}
		

		Sleep(100);

		return 0;
	}
	else {
		std::cout << "Something went wrong. One of the files failed to be opened." << std::endl;

		if (a) fclose(a);
		if (b) fclose(b);
	}


	Sleep(100);
	return 0;
}