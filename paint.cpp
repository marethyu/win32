/* g++ paint.cpp -o paint -lgdi32 -Wl,-subsystem,windows */
// good resource: https://docs.microsoft.com/en-us/windows/win32/inputdev/using-mouse-input

#include <Windows.h>
#include <Windowsx.h>

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 500;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szWndClassName[] = TEXT("MyClass");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

    //Step 1: Registering the Window Class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) 0;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szWndClassName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program only works in Windows NT or greater!"), TEXT("Hey dude!"), MB_OK | MB_ICONERROR);
		return 1;
	}

    // Step 2: Creating the Window
	hwnd = CreateWindow(szWndClassName, /* window class name */
						TEXT("Paint"), /* window title (or caption) */
						WS_OVERLAPPEDWINDOW, /* window style */
						CW_USEDEFAULT, /* initial x position */
						CW_USEDEFAULT, /* initial y position */
						WINDOW_WIDTH, /* initial window width */
						WINDOW_HEIGHT, /* initial window height */
						NULL, /* parent window handle */
						NULL, /* window menu handle */
						hInstance, /* program instance handle */
						NULL); /* creation parameters */

    if (hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

    // Step 3: The Message Loop (heart)
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

// Step 4: the Window Procedure (brain)
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; // handle to device context
    static POINT prev;
    static POINT cur;

    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            prev.x = GET_X_LPARAM(lParam);
            prev.y = GET_Y_LPARAM(lParam);
            break;
        }
        case WM_MOUSEMOVE:
        {
            // When moving the mouse, the user must hold down 
            // the left mouse button to draw lines. 
            if (wParam & MK_LBUTTON)
            {
                hdc = GetDC(hwnd);

                cur.x = GET_X_LPARAM(lParam);
                cur.y = GET_Y_LPARAM(lParam);
                MoveToEx(hdc, prev.x, prev.y, (LPPOINT) NULL);
                LineTo(hdc, cur.x, cur.y);

                prev = cur;
                ReleaseDC(hwnd, hdc);
            } 
            break;
        }
        case WM_LBUTTONUP:
            break;
        case WM_DESTROY:
            PostQuitMessage(0); 
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}