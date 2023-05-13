#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#endif

int close_process_by_name(const std::wstring& name) {
    int count = 0;
#ifdef _WIN32
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, name.c_str()) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess != NULL) {
                    if (TerminateProcess(hProcess, 0)) {
                        count++;
                    }
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
#else
    DIR* dirp = opendir("/proc");
    if (dirp != NULL) {
        struct dirent* dp;
        while ((dp = readdir(dirp)) != NULL) {
            int pid = atoi(dp->d_name);
            if (pid != 0) {
                std::wstring exe_path = L"/proc/" + std::to_wstring(pid) + L"/exe";
                wchar_t buf[PATH_MAX];
                ssize_t len = readlink(exe_path.c_str(), (char*)buf, sizeof(buf) - 1);
                if (len != -1) {
                    buf[len] = '\0';
                    std::wstring exe_name = wcsrchr(buf, '/') + 1;
                    if (_wcsicmp(exe_name.c_str(), name.c_str()) == 0) {
                        if (kill(pid, SIGTERM) == 0) {
                            count++;
                        }
                    }
                }
            }
        }
        closedir(dirp);
    }
#endif
    return count;
}

void Print(std::string text, int speed)
{
    for (char value : text)
    {
        std::cout << value << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
    std::cout << std::endl;
}



int main() {
    SetConsoleTitle(L"Kill Roblox Tool");
    std::wstring program_name = L"RobloxPlayerBeta.exe";
    int count = close_process_by_name(program_name);
    std::string message = "Terminated " + std::to_string(count) + " Process";
    Print(message ,25);
    Print("Press any key to exit...", 25);

    // Wait for a key press
    int ch = _getch();

    // Close the program
    return 0;
}
