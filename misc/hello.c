#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MessageBox(NULL, TEXT("Hello World!\n"), TEXT("Hey!"), MB_OK | MB_ICONWARNING);
	
	return 0;
}