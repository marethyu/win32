/* g++ test.cpp -o test -lSDL2 */

#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>
#include <ctime>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

using namespace std;

const int width = 160;
const int height = 144;

SDL_atomic_t active;

int inputLoop(void *a)
{
    int in = 2;

    while (in != 1)
    {
        cout << ">> ";
        cin >> in;
        SDL_AtomicSet(&active, in);
    }

    return 0;
}

int main()
{
    srand(unsigned(time(NULL)));

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(
        "SDL2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width * 3, height * 3,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);

    vector<uint8_t> pixels(width * height * 4, 0);
    SDL_Event event;
    bool quit = false;

    cout << "Inputs: 0 (rerender); 1 (quit the program)" << endl;
    
    SDL_AtomicSet(&active, 2);
    SDL_CreateThread(inputLoop, "InputThread", (void*) NULL);

    while (!quit)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                SDL_AtomicSet(&active, 1);
                break;
            }
        }

        switch (SDL_AtomicGet(&active))
        {
        case 0: // rerender
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // splat down some random pixels
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    int offset = i * width * 4 + j * 4;

                    int r = rand() % 256;
                    int g = rand() % 256;
                    int b = rand() % 256;

                    pixels[offset] = b;
                    pixels[offset + 1] = g;
                    pixels[offset + 2] = r;
                    pixels[offset + 3] = 255;
                }
            }

            SDL_UpdateTexture(
                texture,
                NULL,
                &pixels[0],
                width * 4);

            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_AtomicSet(&active, 2);
            break;
        }
        case 1: // quit
        {
            quit = true;
            break;
        }
        default: // > 2 - nothing
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();

    return 0;
}
