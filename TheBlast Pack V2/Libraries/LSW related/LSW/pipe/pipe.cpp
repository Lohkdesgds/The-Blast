#include "pipe.h"

namespace LSW {
	namespace v4 {
        void Pipe::keep_reading()
        {
            if (prunt) prunt("Process started.");

            std::string block;

            while (keep && (WaitForSingleObject(piProcInfo.hProcess, 1) == WAIT_TIMEOUT)) {

                DWORD dwRead;
                bool bSuccess = false;

                if (ReadFile(g_hChildStd_OUT_Rd, inter_buf, 8, &dwRead, NULL))
                {
                    for (size_t p = 0; p < dwRead; p++) {
                        block += inter_buf[p];
                    }
                }
                for (size_t u = 0; (u = block.find('\n')) != std::string::npos;)
                {
                    std::string sendin = block.substr(0, u);
                    for (size_t k = 0; k < sendin.length(); k++) if (sendin[k] == '\r') sendin.erase(k--);
                    
                    if (block.length() > u) block = block.substr(u + 1);
                    else block.clear();

                    if (prunt) prunt(sendin);
                }
            }
            if (prunt) prunt("Process died or have been left alone.");
            CloseHandle(piProcInfo.hProcess);
        }
        Pipe::~Pipe()
        {
            kill();
        }
        void Pipe::hookPrint(const std::function<void(const std::string)> f)
        {
            prunt = f;
        }
        bool Pipe::launch(char* cmd)
		{
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;

            // Create a pipe for the child process's STDOUT. 
            if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
                return false;

            // Ensure the read handle to the pipe for STDOUT is not inherited.
            if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
                return false;

            ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
            siStartInfo.cb = sizeof(STARTUPINFO);
            siStartInfo.hStdError = g_hChildStd_OUT_Wr;
            siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
            siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
            
            bool good = CreateProcessA(NULL,
                cmd,           // command line 
                NULL,          // process security attributes 
                NULL,          // primary thread security attributes 
                TRUE,          // handles are inherited 
                0,             // creation flags 
                NULL,          // use parent's environment 
                NULL,          // use parent's current directory 
                &siStartInfo,  // STARTUPINFO pointer 
                &piProcInfo);  // receives PROCESS_INFORMATION 

            if (!good) return false; // bad

            CloseHandle(piProcInfo.hThread);

            CloseHandle(g_hChildStd_OUT_Wr);


            //if (!(connec = _popen(cmd, "rt"))) return false;

            if (keep) {
                if (autosav) {
                    keep = false;
                    autosav->join();
                    delete autosav;
                    autosav = nullptr;
                }
            }
            keep = true;
            autosav = new std::thread([&]() { keep_reading(); });
            return true;
		}

		void Pipe::kill()
		{
            keep = false;
            if (autosav) {
                autosav->join();
                delete autosav;
                autosav = nullptr;
            }
		}
	}
}