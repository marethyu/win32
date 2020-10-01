/* An example of using draw pixels and pixel zoom operations in OpenGL
   using the Win32 API. */
/* gcc image.c -o image -lopengl32 -lglu32 -lgdi32 -Wl,--subsystem,windows */

#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include <stdio.h>


HDC hDC;				/* device context */
HPALETTE hPalette = 0;			/* custom palette (if needed) */
unsigned* image;			/* texture image */
int iwidth, iheight, idepth;


void
reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2i(0, 0);
    glPixelZoom((float)width/iwidth, (float)height/iheight);
}

void
display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(iwidth, iheight, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glFlush();
}


LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    static PAINTSTRUCT ps;

    switch(uMsg) {
    case WM_PAINT:
	display();
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	return 0;

    case WM_SIZE:
	reshape(LOWORD(lParam), HIWORD(lParam));
	PostMessage(hWnd, WM_PAINT, 0, 0);
	return 0;

    case WM_CHAR:
	switch (wParam) {
	case 27:			/* ESC key */
	    PostQuitMessage(0);
	    break;
	}
	return 0;

    case WM_PALETTECHANGED:
	if (hWnd == (HWND)wParam)
	    break;
	/* fall through to WM_QUERYNEWPALETTE */

    case WM_QUERYNEWPALETTE:
	if (hPalette) {
	    UnrealizeObject(hPalette);
	    SelectPalette(hDC, hPalette, FALSE);
	    RealizePalette(hDC);
	    return TRUE;
	}
	return FALSE;

    case WM_CLOSE:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

HWND
CreateOpenGLWindow(char* title, int x, int y, int width, int height, 
		   BYTE type, DWORD flags)
{
    int         n, pf;
    HWND        hWnd;
    WNDCLASS    wc;
    LOGPALETTE* lpPal;
    PIXELFORMATDESCRIPTOR pfd;
    static HINSTANCE hInstance = 0;

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
	hInstance = GetModuleHandle(NULL);
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "OpenGL";

	if (!RegisterClass(&wc)) {
	    MessageBox(NULL, "RegisterClass() failed:  "
		       "Cannot register window class.", "Error", MB_OK);
	    return NULL;
	}
    }

    hWnd = CreateWindow("OpenGL", title, WS_OVERLAPPEDWINDOW |
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			x, y, width, height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
	MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
		   "Error", MB_OK);
	return NULL;
    }

    hDC = GetDC(hWnd);

    /* there is no guarantee that the contents of the stack that become
       the pfd are zeroed, therefore _make sure_ to clear these bits. */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
    pfd.iPixelType   = type;
    pfd.cDepthBits   = 32;
    pfd.cColorBits   = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
	MessageBox(NULL, "ChoosePixelFormat() failed:  "
		   "Cannot find a suitable pixel format.", "Error", MB_OK); 
	return 0;
    } 
 
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
	MessageBox(NULL, "SetPixelFormat() failed:  "
		   "Cannot set format specified.", "Error", MB_OK);
	return 0;
    } 

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
	int redMask, greenMask, blueMask, i;

	n = 1 << pfd.cColorBits;
	if (n > 256) n = 256;

	lpPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +
				    sizeof(PALETTEENTRY) * n);
	memset(lpPal, 0, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * n);
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = n;

	GetSystemPaletteEntries(hDC, 0, n, &lpPal->palPalEntry[0]);
    
	/* assume an RGBA pixel type. */
	redMask = (1 << pfd.cRedBits) - 1;
	greenMask = (1 << pfd.cGreenBits) - 1;
	blueMask = (1 << pfd.cBlueBits) - 1;
	
	/* fill in the entries with an RGB color ramp. */
	for (i = 0; i < n; ++i) {
	    lpPal->palPalEntry[i].peRed = 
		(((i >> pfd.cRedShift)   & redMask)   * 255) / redMask;
	    lpPal->palPalEntry[i].peGreen = 
		(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
	    lpPal->palPalEntry[i].peBlue = 
		(((i >> pfd.cBlueShift)  & blueMask)  * 255) / blueMask;
	    lpPal->palPalEntry[i].peFlags = 0;
	}

	hPalette = CreatePalette(lpPal);
	if (hPalette) {
	    SelectPalette(hDC, hPalette, FALSE);
	    RealizePalette(hDC);
	}

	free(lpPal);
    }

    ReleaseDC(hDC, hWnd);

    return hWnd;
}    

int
main(int argc, char** argv)
{
    HGLRC hRC;				/* opengl context */
    HWND  hWnd;				/* window */
    MSG   msg;				/* message */

    if (argc > 1) {
	if (strcmp(argv[1], "-h") == 0) {
	    fprintf(stderr, "%s [file]\n", argv[0]);
	    fprintf(stderr, "  file   Iris RGB format image file\n");
	    exit(0);
	}
	image = read_texture(argv[1], &iwidth, &iheight, &idepth);
    } else {
	image = opengl_image;
	iwidth = opengl_width;
	iheight = opengl_height;
	idepth = opengl_depth;
    }

    hWnd = CreateOpenGLWindow(argv[0], 0, 0, 256, 256, 
			      PFD_TYPE_RGBA, 0);
    if (hWnd == NULL)
	exit(1);

    hDC = GetDC(hWnd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

#if 0 /* code to create a header file out of an image. */
    printf("int _width = %d;\n", iwidth);
    printf("int _height = %d;\n", iheight);
    printf("int _depth = %d;\n", idepth);
    printf("unsigned _image[] = {\n");
    for (j = 0; j < iheight; j++) {
 	for (i = 0; i < iwidth; i++) {
 	    printf("0x%x, ", image[j * iwidth + i]);
 	}
 	printf("\n");
    }
    printf("};\n");
    exit(0);
#endif

    ShowWindow(hWnd, SW_SHOW);

    while(GetMessage(&msg, hWnd, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    wglMakeCurrent(NULL, NULL);
    ReleaseDC(hDC, hWnd);
    wglDeleteContext(hRC);
    DestroyWindow(hWnd);
    if (hPalette)
	DeleteObject(hPalette);

    return 0;
}