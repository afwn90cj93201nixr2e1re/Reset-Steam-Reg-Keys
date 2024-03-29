#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <Psapi.h>

std::wstring Steam = L"Steam.exe";
std::string DllName = "steamclient.dll";
char g_LibraryPath[MAX_PATH] = { 0 };

DWORD FindProcess(std::wstring &processName);
void SetActiveProcess(int pid);
void GetPath(int processID);
void SetSteamClientDll(const char *pszLib);
int main(){
	auto pid = FindProcess(Steam);
	GetPath(pid);
	SetActiveProcess(pid);
	SetSteamClientDll(DllName.c_str());
    return 0;
}
void GetPath(int processID) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,processID);
	GetModuleFileNameExA(hProcess,NULL, g_LibraryPath, sizeof(g_LibraryPath));
	char *psz = strrchr(g_LibraryPath, '\\') + 1;
	*psz = '\0';
	DllName.insert(0, g_LibraryPath);
}
DWORD FindProcess(std::wstring &processName){
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)return 0;
	while (Process32Next(processesSnapshot, &processInfo)){
		if (!processName.compare(processInfo.szExeFile)){
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}
	CloseHandle(processesSnapshot);
	return 0;
}
void SetActiveProcess(int pid){
	HKEY phkResult;
	RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess", 0, 0x20006, &phkResult);
	RegSetValueExA(phkResult, "pid", 0, 4, (unsigned char *)&pid, 4);
	RegCloseKey(phkResult);
}

void SetSteamClientDll(const char *pszLib){
	DWORD dwD;
	HKEY phkResult;
	RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess", 0, 0x20006, &phkResult);
	RegSetValueExA(phkResult, "SteamClientDll", 0, 1, (unsigned char *)pszLib, strlen(pszLib) + 1);
	RegCloseKey(phkResult);
}
