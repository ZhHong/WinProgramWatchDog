#include<Windows.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>
#include <vector>
#include "log.h"

DWORD GetProcessIdFromName(const char*processName)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	char pname[300];
	do
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		//把WCHAR*类型转换为const char*类型  
		sprintf_s(pname, "%ws", pe.szExeFile);
		//比较两个字符串，如果找到了要找的进程  
		if (strcmp(pname, processName) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}

	} while (1);

	CloseHandle(hSnapshot);

	return id;
}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

bool ReadInitFile(std::vector<std::string> &watch_list) {
	try
	{

		std::ifstream ifs("./watch_dog.ini");
		if (!ifs.is_open()) {
			watch_dog_log::traceError("can not find init file. check ./watch_dog.ini exsists!");
			return false;
		}
		while (!ifs.eof()) {
			char buffer[256];
			ifs.getline(buffer, 256);
			std::string s = buffer;
			watch_list.push_back(s);
		}
	}
	catch (const std::exception& e)
	{
		watch_dog_log::traceError((char *)(e.what()));
		return false;
	}
	return true;
}

int main() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	printf("Welcome WatchDog ! Powered by ZhouHong @ 2017  Version Beta 0.0.1 \n");


	std::vector<std::string> watch_list;
	std::vector<std::string> die_list;

	//如果加载配置失败，直接退出
	if (!ReadInitFile(watch_list)) {
		watch_dog_log::traceError("Read init file filed..........");
		return 0;
	}

	auto it = watch_list.begin();
	for (it; it != watch_list.end(); it++) {
		PROCESS_INFORMATION pi;
		STARTUPINFO si = { sizeof(si) };
		watch_dog_log::traceInfo("start program [%s] !",it->data());
		//TEXT将const char*类型转换为LPCWSTR类型 
		BOOL ret = CreateProcess(stringToLPCWSTR(it->data()), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		watch_dog_log::traceInfo("Dog watch [%s]  at %d",it->data(), pi.dwProcessId);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	do
	{
		//check process alive
		int pid = 0;
		auto lit = watch_list.begin();
		for (lit; lit != watch_list.end(); lit++) {
			pid = GetProcessIdFromName(lit->data());
			if (pid == 0) {
				watch_dog_log::traceError("[%s] die !", lit->data());
				die_list.push_back(lit->data());
			}
		}
		//if has die proccess restart it
		if (die_list.size() > 0) {
			auto dit = die_list.begin();
			for (dit; dit != die_list.end(); dit++) {
				PROCESS_INFORMATION pi1;
				STARTUPINFO si1= { sizeof(si1) };
				BOOL result = CreateProcess(stringToLPCWSTR(dit->data()), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si1, &pi1);
				watch_dog_log::traceInfo("restart [%s] run at %d",dit->data(),pi1.dwProcessId);
				CloseHandle(pi1.hThread);
				CloseHandle(pi1.hProcess);
			}
			die_list.clear();
		}
		Sleep(10000);
	} while (1);
}