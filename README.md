# win32

My Win32 and SDL2 playground. Most of the programs are tailored for creating emulators and games. Use them at your own risk. They are not guaranteed to be bug-free. Please open an issue if there are any.

## Useful programs

- [mandelbrot.c](mandelbrot.c): Render a Mandelbrot set in a Win32 window.

- [snake.cpp](snake.cpp): Win32 snake game. Demonstrates Win32 timer and bitmap usage.

- [paint.cpp](paint.cpp): Simple paint app.

- [sdl_fast_render.cpp](sdl_fast_render.cpp): SDL2 rendering with textures.

- [sdlwin_tex.cpp](sdlwin_tex.cpp): Basically it is same as sdl_fast_render.cpp but is enhanced with a Win32 menubar. Demonstrates one way to retrive window handle from a SDL window.

- [sdl_tex_rewrite_rev.cpp](sdl_tex_rewrite_rev.cpp): Improved version of sdlwin_tex.cpp. Shows how to make SDL window a *child* of a Win32 window.

- [sdl_win_console_sync.cpp](sdl_win_console_sync.cpp): Shows how to sync terminal with SDL window. Useful for creating emulator debuggers.