/* gcc mandelbrot.c -o mandelbrot -lgdi32 -Wl,-subsystem,windows */

#include <math.h>
#include <stdlib.h>
#include <windows.h>

typedef struct
{
    double real;
    double imag;
} Complex;

const int win_width = 500;
const int win_height = 450;

const int max_iter = 200;

const double zoomX = 160.0;
const double zoomY = 160.0;

Complex *new(double real, double imag)
{
    Complex *cplx = (Complex *) malloc(sizeof(Complex));
    
    cplx->real = real;
    cplx->imag = imag;
    
    return cplx;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static RECT rect;
    static int width, height;
    static COLORREF *display;
    
    switch (msg)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        display = (COLORREF *) malloc(sizeof(COLORREF) * width * height);
        COLORREF *ptr = display;
        
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                Complex *z = new(0.0, 0.0);
                Complex *c = new((x - width / 2) / zoomX, (y - height / 2) / zoomY);
                
                int iter = 0;
                
                while (z->real * z->real + z->imag * z->imag < 4.0 && iter < max_iter)
                {
                    double tmp = z->real * z->real - z->imag * z->imag + c->real;
                    z->imag = z->real * z->imag + z->real * z->imag + c->imag;
                    z->real = tmp;
                    iter++;
                }
                
                int r = (int) ceil((double) iter / max_iter * 255.0);
                int g = (int) ceil((double) iter / max_iter * 255.0);
                int b = (int) ceil((double) iter / max_iter * 255.0);
                
                *ptr++ = (b << 16) | (g << 8) | r; // the colour will be different shades of gray
                
                free(z);
                free(c);
            }
        }
        
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC, memDC;
        HBITMAP hBmp, hOldBmp;
        
        hDC = BeginPaint(hWnd, &ps);
        memDC = CreateCompatibleDC(hDC);
        hBmp = CreateBitmap(width, height, 1, 32, (void *) display);
        hOldBmp = (HBITMAP) SelectObject(memDC, hBmp);
        
        BitBlt(hDC, rect.left, rect.top, width, height, memDC, 0, 0, SRCCOPY);
        
        SelectObject(memDC, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(memDC);
        
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
        TEXT("Mandelbrot Set"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, win_width, win_height,
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