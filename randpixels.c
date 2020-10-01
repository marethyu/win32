/* gcc randpixels.c -o randpixels -lgdi32 -Wl,-subsystem,windows */

#include <stdlib.h>
#include <time.h>
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        srand((unsigned int) time(NULL));
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rect;
        HDC hDC, memDC;
        HBITMAP hBmp, hOldBmp;
        int width, height;
        
        hDC = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        memDC = CreateCompatibleDC(hDC); // device context for background buffer
        hBmp = CreateCompatibleBitmap(hDC, width, height); // background bitmap (this's where we will do most of the drawing)
        hOldBmp = (HBITMAP) SelectObject(memDC, hBmp); // save the old bitmap to prevent GDI leaks
        
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int red = rand() % 256;
                int green = rand() % 256;
                int blue = rand() % 256;
                
                SetPixel(memDC, x, y, RGB(red, green, blue)); // to draw on the background buffer, we use memDC instead of hDC
            }
        }
        
        BitBlt(hDC, 0, 0, width, height, memDC, 0, 0, SRCCOPY); // copy the result into the target DC
        
        // clean up
        SelectObject(memDC, hOldBmp); // select back to the original bitmap
        DeleteObject(hBmp);
        DeleteDC(memDC); /* we no longer need these shit so delete them */
        
        EndPaint(hWnd, &ps);
        
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
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
        return 0;
    }
    
    hWnd = CreateWindow(szClassName,
        TEXT("Random Pixels"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300,
        NULL, NULL, hInstance, NULL);
    
    if (hWnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int) msg.wParam;
}