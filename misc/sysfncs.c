#include <windows.h>
#include <Lmcons.h>
#include <tchar.h>
#include <stdio.h>

// Useful utility function for debugging
int CDECL Debug(TCHAR *szTitle, TCHAR *szFormat, ...)
{
	TCHAR *szBuffer[100];
	va_list pArgList;
	
	va_start(pArgList, szFormat); // equal to pArgList = (char *) &szFormat + sizeof(szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);
	
	return MessageBox(NULL, szBuffer, szTitle, MB_OK | MB_ICONWARNING);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	// Get screen (monitor) dimensions
	
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	
	Debug(TEXT("Screen Size"), TEXT("Your screen size is %d pixels wide by %d high."), cxScreen, cyScreen);
	
	// Get name of your computer
	
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwLength = sizeof(szComputerName) / sizeof(TCHAR);
	
	if (!GetComputerName(szComputerName, &dwLength))
	{
		Debug(TEXT("Uh..."), TEXT("Unable to get computer name!"));
	}
	else
	{
		Debug(TEXT("Computer Name"), TEXT("Your computer name is %s."), szComputerName);
	}
	
	// Get your username
	
	TCHAR szUserName[UNLEN + 1];
	DWORD dwLen = sizeof(szUserName) / sizeof(TCHAR);
	
	if (!GetUserName(szUserName, &dwLen))
	{
		Debug(TEXT("Uh..."), TEXT("Unable to get username!"));
	}
	else
	{
		Debug(TEXT("User Name"), TEXT("Your username is %s."), szUserName);
	}
	
	// Get your current directory
	
	TCHAR szCurrentDirectory[MAX_PATH];
	
	// memory stats
	
	MEMORYSTATUSEX mem = {0};
	
	mem.dwLength = sizeof(mem);
	
	if (!GlobalMemoryStatusEx(&mem))
	{
		Debug(TEXT("Uh..."), TEXT("Unable to get memory status!"));
	}
	else
	{
		Debug(TEXT("Memory Status"), TEXT("memory in use: %ld percent\ntotal physical memory: %lld\nfree physical memory: %lld\ntotal virtual memory: %lld\nfree virtual memory: %lld"), mem.dwMemoryLoad, mem.ullTotalPhys, mem.ullAvailPhys, mem.ullTotalVirtual, mem.ullAvailVirtual);
	}
	
	return 0;
}