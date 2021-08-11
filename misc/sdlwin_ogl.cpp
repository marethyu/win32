/* g++ sdlwin_ogl.cpp -o sdlwin_ogl -std=c++14 -lSDL2 -lopengl32 -Wl,-subsystem,windows */
/* buggy */

#include <cstdlib>
#include <ctime>
#include <Windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_syswm.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_SCALE_FACTOR 2

#define ID_LOADROM 0
#define ID_EXIT 1
#define ID_ABOUT 2

void InitGL()
{
    glViewport(0, 0, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR, 0, -1.0, 1.0);
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glDisable(GL_BLEND);
}

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

void RenderScreen(SDL_Window *wnd, uint32_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH][3])
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glLoadIdentity();
 	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
 	glDrawPixels(SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	SDL_GL_SwapWindow(wnd);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SDL_Window *wnd;
    SDL_GLContext glcontext;
    HWND hWnd;
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(1);
    }
    
    InitGL();
    
    wnd = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCREEN_SCALE_FACTOR, SCREEN_HEIGHT * SCREEN_SCALE_FACTOR, SDL_WINDOW_OPENGL);
    
    if (wnd == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        std::exit(1);
    }
    
    glcontext = SDL_GL_CreateContext(wnd);
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
    uint32_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH][3] = {};
    
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
                    int r = std::rand() % 256;
                    int g = std::rand() % 256;
                    int b = std::rand() % 256;
                    
                    if (flag)
                    {
                        pixels[i][j][0] = r;
                        pixels[i][j][1] = g;
                        pixels[i][j][2] = b;
                    }
                    else
                    {
                        pixels[i][j][0] = r;
                        pixels[i][j][1] = r;
                        pixels[i][j][2] = r;
                    }
                }
            }
        }
        
        RenderScreen(wnd, pixels);
    }
    
    SDL_DestroyWindow(wnd);
    wnd = nullptr;
    
    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();
    
    return 0;
}