/* g++ sdlwin_tex.cpp -o sdlwin_tex -std=c++14 -lSDL2 -Wl,-subsystem,windows */

#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_SCALE_FACTOR 2

#define ID_LOADROM 0
#define ID_EXIT 1
#define ID_ABOUT 2

HWND GetSDLWndHandle(SDL_Window *wnd)
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    
    if (!SDL_GetWindowWMInfo(wnd, &info))
    {
        return nullptr;
    }
    
    return info.info.win.window;
}

void InitMenu(HWND hWnd)
{
    HMENU hMenuBar = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();
    
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hFile, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hHelp, "Help");
    
    AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
    AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");
    
    AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");
    
    SetMenu(hWnd, hMenuBar);
}

void RenderScreen(SDL_Renderer *renderer, SDL_Texture *texture, const std::vector<unsigned char>& pixels)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, &pixels[0], SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SDL_Window *wnd;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    HWND hWnd;
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(1);
    }
    
    wnd = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR, SDL_WINDOW_SHOWN);
    
    if (wnd == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        std::exit(1);
    }
    
    renderer = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
    
    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        std::exit(1);
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    if (texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
        std::exit(1);
    }
    
    hWnd = GetSDLWndHandle(wnd);
    
    if (hWnd == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot retrieve hwnd");
        std::exit(1);
    }
    
    InitMenu(hWnd);
    SDL_SetWindowSize(wnd, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR); // resize because we just added the menubar
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    
    SDL_Event evt;
    bool quit = false;
    bool started = false;
    bool flag = true;
    std::vector<unsigned char> pixels(SCREEN_WIDTH * SCREEN_HEIGHT * 4, 0);
    
    while (!quit)
    {
        while (SDL_PollEvent(&evt) != 0)
        {
            switch (evt.type)
            {
            case SDL_SYSWMEVENT:
                switch (evt.syswm.msg->msg.win.msg)
                {
                case WM_COMMAND:
                    switch (LOWORD(evt.syswm.msg->msg.win.wParam))
                    {
                    case ID_LOADROM:
                        started = true;
                        break;
                    case ID_EXIT:
                        quit = true;
                        break;
                    }
                    break;
                }
                break;
            case SDL_KEYDOWN:
                switch (evt.key.keysym.sym)
                {
                case SDLK_q:
                    if (started)
                    {
                        flag = !flag;
                    }
                    break;
                }
                break;
            case SDL_WINDOWEVENT_CLOSE:
                evt.type = SDL_QUIT;
                SDL_PushEvent(&evt);
                break;
            case SDL_QUIT:
                quit = true;
                break;
            }
        }
        
        if (started)
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
                    
                    if (flag)
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
        
        RenderScreen(renderer, texture, pixels);
    }
    
    SDL_DestroyTexture(texture);
    texture = nullptr;
    
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    
    SDL_DestroyWindow(wnd);
    wnd = nullptr;
    
    SDL_Quit();
    
    return 0;
}