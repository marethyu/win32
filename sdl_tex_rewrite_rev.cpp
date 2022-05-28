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
public:
    SDLApp(int width, int height);
    ~SDLApp();

    void Create(HWND hWnd);
    void UpdatePixels();
    void Paint();
    void HandleMenu(WPARAM wParam);
    void HandleKey(WPARAM wParam);
    void CleanUp();
    void Destroy();
private:
    HWND hwnd;

    SDL_Window *wnd;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    bool flag;

    int width;
    int height;

    std::vector<uint32_t> pixels;
};

SDLApp::SDLApp(int width, int height)
  : width(width), height(height), pixels(width * height)
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

    renderer = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        std::exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
        std::exit(1);
    }

    HMENU hMenuBar = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hFile, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hHelp, "Help");

    AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
    AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");

    AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");

    SetMenu(hWnd, hMenuBar);

    hwnd = hWnd;
}

void SDLApp::UpdatePixels()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int offset = i * width + j;

            uint8_t r = std::rand() % 256;
            uint8_t g = flag ? r : std::rand() % 256;
            uint8_t b = flag ? r : std::rand() % 256;

            uint32_t bgra = (0xFF << 24) | (r << 16) | (g << 8) | b;

            pixels[offset] = bgra;
        }
    }
}

void SDLApp::Paint()
{
    SDL_UpdateTexture(texture, NULL, &pixels[0], width * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void SDLApp::HandleMenu(WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_LOADROM:
        if(!SetTimer(hwnd, ID_TIMER, UPDATE_INTERVAL, NULL))
        {
            MessageBox(hwnd, "Could not set timer!", "Error", MB_OK | MB_ICONEXCLAMATION);
            PostQuitMessage(1);
        }
        break;
    case ID_EXIT:
        PostQuitMessage(0);
        break;
    }
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
    KillTimer(hwnd, ID_TIMER);
}

void SDLApp::Destroy()
{
    PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT);

    switch (msg)
    {
    case WM_CREATE:
    {
        app.Create(hWnd);
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
        app.HandleMenu(wParam);
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
    RECT rcClient;
    UINT style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE; // no maximize box and resizing

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

    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = SCREEN_WIDTH * SCREEN_SCALE_FACTOR;
    rcClient.bottom = SCREEN_HEIGHT * SCREEN_SCALE_FACTOR;

    AdjustWindowRectEx(&rcClient, style, TRUE, 0);

    hWnd = CreateWindow(szClassName,
        TEXT("SDLWin1"),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        NULL,
        NULL,
        hInstance,
        NULL);

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
