/* g++ hellowin.cpp -o hellowin -Wl,-subsystem,windows */

#include <Windows.h>

const int width = 500;
const int height = 400;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szWndClassName[] = TEXT("hellowin");
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
    wndclass.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szWndClassName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program only works in Windows NT or greater!"), TEXT("Hey dude!"), MB_OK | MB_ICONERROR);
        return 1;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindow(szWndClassName, /* window class name */
                        TEXT("Hey!"), /* window title (or caption) */
                        WS_OVERLAPPEDWINDOW, /* window style */
                        CW_USEDEFAULT, /* initial x position */
                        CW_USEDEFAULT, /* initial y position */
                        width, /* initial window width */
                        height, /* initial window height */
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        GetClientRect(hwnd, &rect);
        DrawText(hdc, TEXT("Hello World!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}