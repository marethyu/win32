/* gcc sdlwin.c -o sdlwin -lSDL2 -Wl,-subsystem,windows */

#include <windows.h>

#include <SDL2/SDL.h>

const int win_width = 500;
const int win_height = 450;

int b = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SDL_Window *sdlWnd;
    SDL_Surface *surface;

    switch (msg)
    {
    case WM_CREATE:
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            MessageBox(hWnd, TEXT("SDL cannot be initialized!"), TEXT("Error!"),
                MB_ICONEXCLAMATION | MB_OK);
            PostQuitMessage(0);
        }

        sdlWnd = SDL_CreateWindowFrom(hWnd);

        if (sdlWnd == NULL)
        {
            MessageBox(hWnd, TEXT("SDL window cannot be created!"), TEXT("Error!"),
                MB_ICONEXCLAMATION | MB_OK);
            SDL_Quit();
            PostQuitMessage(0);
        }

        SDL_SetWindowTitle(sdlWnd, "SDL Window");

        break;
    case WM_PAINT:
        surface = SDL_GetWindowSurface(sdlWnd);
        SDL_FillRect(surface, &surface->clip_rect, b ? 0xFFFF00FF : 0xFFFF0000);
        SDL_UpdateWindowSurface(sdlWnd);
        break;
    case WM_CLOSE:
        SDL_DestroyWindow(sdlWnd);
        SDL_Quit();
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
        return 1;
    }

    hWnd = CreateWindow(szClassName,
        TEXT("SDLWin"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, win_width, win_height,
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SDL_Event e;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)
            {
                b = !b;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}