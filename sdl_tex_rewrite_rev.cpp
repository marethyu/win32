/* g++ sdl_tex_rewrite_rev.cpp -o sdl_tex_rewrite_rev -lSDL2 */

#include <vector>

#include <cstdlib>
#include <ctime>

#include <Windows.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_SCALE_FACTOR 3

#define ID_LOADROM 0
#define ID_EXIT 1
#define ID_ABOUT 2

#define ID_TIMER 1
#define UPDATE_INTERVAL 17

class SDLApp
{
private:
    SDL_Window *wnd;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    bool flag;
    std::vector<uint8_t> pixels;
public:
    SDLApp();
    ~SDLApp();

    void Create(HWND);
    void FixSize();
    void UpdatePixels();
    void Paint();
    void HandleKey(WPARAM wParam);
    void CleanUp();
    void Destroy();
};

SDLApp::SDLApp() : pixels(SCREEN_WIDTH * SCREEN_HEIGHT * 4)
{
    flag = false;
}

SDLApp::~SDLApp()
{}

void SDLApp::Create(HWND hWnd)
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(1);
    }

    wnd = SDL_CreateWindowFrom(hWnd);
    if (wnd == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        std::exit(1);
    }
    SDL_SetWindowTitle(wnd, "SDL App");

    renderer = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        std::exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
        std::exit(1);
    }
}

void SDLApp::FixSize()
{
    SDL_SetWindowSize(wnd, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR);
}

void SDLApp::UpdatePixels()
{
    int w = SCREEN_WIDTH;
    int h = SCREEN_HEIGHT;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int offset = i * w * 4 + j * 4;

            int r = std::rand() % 256;
            int g = std::rand() % 256;
            int b = std::rand() % 256;

            if (!flag)
            {
                pixels[offset] = b;
                pixels[offset + 1] = g;
                pixels[offset + 2] = r;
                pixels[offset + 3] = 255;
            }
            else
            {
                pixels[offset] = r;
                pixels[offset + 1] = r;
                pixels[offset + 2] = r;
                pixels[offset + 3] = 255;
            }
        }
    }
}

void SDLApp::Paint()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, NULL, &pixels[0], SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void SDLApp::HandleKey(WPARAM wParam)
{
    if (wParam == 0x51) // q key
    {
        flag = !flag;
    }
}

void SDLApp::CleanUp()
{
    SDL_DestroyTexture(texture);
    texture = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(wnd);
    wnd = NULL;

    SDL_Quit();
}

void SDLApp::Destroy()
{
    PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SDLApp app;

    switch (msg)
    {
    case WM_CREATE:
    {
        app.Create(hWnd);

        HMENU hMenuBar = CreateMenu();
        HMENU hFile = CreatePopupMenu();
        HMENU hHelp = CreatePopupMenu();

        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hFile, "File");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hHelp, "Help");

        AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
        AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");

        AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");

        SetMenu(hWnd, hMenuBar);

        app.FixSize(); // resize because we just added the menubar
        break;
    }
    case WM_TIMER:
    {
        app.UpdatePixels();
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_PAINT:
    {
        app.Paint();
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_LOADROM:
            if(!SetTimer(hWnd, ID_TIMER, UPDATE_INTERVAL, NULL))
            {
                MessageBox(hWnd, "Could not set timer!", "Error", MB_OK | MB_ICONEXCLAMATION);
                PostQuitMessage(1);
            }
            break;
        case ID_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    }
    case WM_KEYDOWN:
    {
        app.HandleKey(wParam);
        break;
    }
    case WM_CLOSE:
    {
        app.CleanUp();
        KillTimer(hWnd, ID_TIMER);
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY:
        app.Destroy();
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
        TEXT("SDLWin"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR,
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
