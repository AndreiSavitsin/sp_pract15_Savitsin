#include <iostream>
#include <Windows.h>
#include <fstream>


struct ProcInfo
{
	DWORD PID;
	HANDLE hProcess;
	HANDLE hThread;
	std::string state;
};

ProcInfo* processes[3];
int procCount = 0;

int main()
{
	setlocale(LC_ALL, "Russian");

	wchar_t app[] = L"C:\\Users\\st310-02\\Desktop\\Савицин ПР-31\\sp_pract15_Savitsin\\x64\\Debug\\sp_pract15_exemplar";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	for (int i = 0; i < 3; i++)
	{
		if (!CreateProcess(app, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			std::cout << "Дочерний процесс не создался!\n";
			return GetLastError();
		}
		else
		{
			std::cout << "Процесс запущен. PID: " << pi.dwProcessId << std::endl;
			processes[i] = new ProcInfo;
			processes[i]->PID = pi.dwProcessId;
			processes[i]->hProcess = pi.hProcess;
			processes[i]->hThread = pi.hThread;
			procCount++;			
		}
	}	

	for (int i = 0; procCount; i++)
	{
		CloseHandle(processes[i]->hProcess);
		delete processes[i];
	}

	return 0;
}