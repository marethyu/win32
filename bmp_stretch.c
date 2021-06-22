/* compile: gcc test.c -o test -lgdi32 -Wl,-subsystem,windows */
// TODO fast way to update? resize pixel size? best coding practices?

#include <windows.h>

#define WND_WIDTH 500
#define WND_HEIGHT 500

#define ID_TIMER 1

void DrawBitmap(HDC hdc, RECT* rect, BITMAPINFO info, void* bmpMem)
{
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;

    StretchDIBits(
        hdc,
        0,
        0,
        width,
        height,
        0,
        0,
        info.bmiHeader.biWidth,
        info.bmiHeader.biHeight,
        bmpMem,
        &info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

void UpdateBitmap(BITMAPINFO info, void *bmpMem, POINT pix)
{
    int width = info.bmiHeader.biWidth;
    int height = info.bmiHeader.biHeight;

    BYTE *pixel = (BYTE *) bmpMem;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x == pix.x && y == pix.y)
            {
                *pixel++ = 0;   /* blue */
                *pixel++ = 0;   /* green */
                *pixel++ = 255; /* red */
                *pixel++ = 255; /* alpha */
            }
            else
            {
                *pixel++ = 0;   /* blue */
                *pixel++ = 0;   /* green */
                *pixel++ = 0;   /* red */
                *pixel++ = 255; /* alpha */
            }
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static BITMAPINFO info;
    static void *bmpMem;
    static POINT pixel;

    switch (msg)
    {
    case WM_CREATE:
    {
        //RECT rcClient;

        //GetClientRect(hWnd, &rcClient);

        int width = 30;
        int height = 30;

        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = 0;
        info.bmiHeader.biXPelsPerMeter = 0;
        info.bmiHeader.biYPelsPerMeter = 0;
        info.bmiHeader.biClrUsed = 0;
        info.bmiHeader.biClrImportant = 0;

        bmpMem = VirtualAlloc(0, width * height * 4, MEM_COMMIT, PAGE_READWRITE);

        pixel.x = width / 2;
        pixel.y = height / 2;

        if(!SetTimer(hWnd, ID_TIMER, 50, NULL))
        {
            MessageBox(hWnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
            PostQuitMessage(1);
        }

        break;
    }/*
    case WM_PAINT:
    {
        RECT rcClient;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &rcClient);
        DrawBitmap(hdc, &rcClient, info, bmpMem);

        EndPaint(hWnd, &ps);
        break;
    }*/
    case WM_TIMER:
    {
        RECT rcClient;
        HDC hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rcClient);
        UpdateBitmap(info, bmpMem, pixel);
        DrawBitmap(hdc, &rcClient, info, bmpMem);

        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_LEFT: pixel.x -= 1; break;
        case VK_RIGHT: pixel.x += 1; break;
        case VK_UP: pixel.y += 1; break;
        case VK_DOWN: pixel.y -= 1; break;
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, ID_TIMER);
        VirtualFree(bmpMem, 0, MEM_RELEASE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const TCHAR szClassName[] = TEXT("MyClass");

    WNDCLASS wc;
    HWND hWnd;
    MSG msg;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szClassName;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    hWnd = CreateWindow(szClassName,
                        TEXT("Title"),
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, /* this window style prevents window resizing */
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        WND_WIDTH,
                        WND_HEIGHT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    if (hWnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}