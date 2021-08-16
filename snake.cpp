/* g++ snake.cpp -o snake -lgdi32 -Wl,-subsystem,windows */
// TODO https://codereview.stackexchange.com/questions/263298/win32-snake-game/263328#263328, std::random

#include <deque>
#include <vector>

#include <cstdio>
#include <ctime>

#include <Windows.h>

#define ID_TIMER 1

constexpr int ROWS = 40;
constexpr int COLS = 40;
constexpr int SCREEN_SCALE_FACTOR = 10;

struct Cell
{
    int row, col;
    Cell(int row, int col) : row(row), col(col)
    {}
};

class Snake
{
public:
    Snake();
    ~Snake() = default;
    
    void Update(HWND hWnd);
    void Paint(HWND hWnd);
    void HandleKey(WPARAM wParam);
private:
    enum Direction { UP = 0, DOWN, LEFT, RIGHT };
    enum CellType { EMPTY = 0, SNAKE, FOOD };
    
    std::vector<BYTE> pixels;
    std::deque<Cell> snake;
    
    BITMAPINFO info;
    
    int dir;
    
    void MakeFood();
    void SetCell(int row, int col, CellType type);
    
    CellType GetCellType(int row, int col);
    Cell Move(const Cell& cell);
};

Snake::Snake()
{
    pixels.resize(ROWS * COLS * 4);
    srand(static_cast<unsigned int>(time(NULL)));
    Snake::MakeFood();
    snake.push_back(Cell(ROWS / 2, COLS / 2));
    dir = UP;
    
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = COLS;
    info.bmiHeader.biHeight = ROWS;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biXPelsPerMeter = 0;
    info.bmiHeader.biYPelsPerMeter = 0;
    info.bmiHeader.biClrUsed = 0;
    info.bmiHeader.biClrImportant = 0;
}

void Snake::Update(HWND hWnd)
{
    snake.push_back(Snake::Move(snake.back()));
    
    Cell head = snake.back();
    bool eaten = false;
    
    switch (Snake::GetCellType(head.row, head.col))
    {
    case FOOD:
    {
        snake.push_back(Snake::Move(head));
        Cell newHead = snake.back();
        Snake::SetCell(newHead.row, newHead.col, SNAKE);
        eaten = true;
        break;
    }
    case SNAKE:
    {
        char buff[200];
        snprintf(buff, sizeof(buff), "Score: %d", snake.size() - 1);
        KillTimer(hWnd, ID_TIMER);
        MessageBox(hWnd, TEXT(buff), "Game Over!", MB_OK | MB_ICONINFORMATION);
        DestroyWindow(hWnd);
        break;
    }
    default:
        break;
    }
    
    Snake::SetCell(head.row, head.col, SNAKE);
    
    Cell tail = snake.front();
    snake.pop_front();
    Snake::SetCell(tail.row, tail.col, EMPTY);
    
    if (eaten) Snake::MakeFood();
}

void Snake::Paint(HWND hWnd)
{
    RECT rcClient;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    
    GetClientRect(hWnd, &rcClient);
    
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;
    
    StretchDIBits(hdc,
                  0,
                  0,
                  width,
                  height,
                  0,
                  0,
                  COLS,
                  ROWS,
                  &pixels[0],
                  &info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
    
    EndPaint(hWnd, &ps);
}

void Snake::HandleKey(WPARAM wParam)
{
    switch (wParam)
    {
    case VK_UP:
        dir = UP;
        break;
    case VK_DOWN:
        dir = DOWN;
        break;
    case VK_LEFT:
        dir = LEFT;
        break;
    case VK_RIGHT:
        dir = RIGHT;
        break;
    default:
        break;
    }
}

void Snake::MakeFood()
{
    int row = rand() % ROWS;
    int col = rand() % COLS;
    
    while (Snake::GetCellType(row, col) == SNAKE)
    {
        row = rand() % ROWS;
        col = rand() % COLS;
    }
    
    Snake::SetCell(row, col, FOOD);
}

void Snake::SetCell(int row, int col, CellType type)
{
    int offset = row * COLS * 4 + col * 4;
    
    switch (type)
    {
    case EMPTY:
        pixels[offset]     = 0;
        pixels[offset + 1] = 0;
        pixels[offset + 2] = 0;
        pixels[offset + 3] = 255;
        break;
    case SNAKE:
        pixels[offset]     = 0;
        pixels[offset + 1] = 255;
        pixels[offset + 2] = 0;
        pixels[offset + 3] = 255;
        break;
    case FOOD:
        pixels[offset]     = 0;
        pixels[offset + 1] = 0;
        pixels[offset + 2] = 255;
        pixels[offset + 3] = 255;
        break;
    default:
        break;
    }
}

Snake::CellType Snake::GetCellType(int row, int col)
{
    int offset = row * COLS * 4 + col * 4;
    
    int red = pixels[offset + 2];
    int green = pixels[offset + 1];
    int blue  = pixels[offset];
    
    if (red == 0 && green == 0 && blue == 0) return EMPTY;
    if (red == 0 && green == 255 && blue == 0) return SNAKE;
    if (red == 255 && green == 0 && blue == 0) return FOOD;
    
    return EMPTY; // should never reach here
}

Cell Snake::Move(const Cell& cell)
{
    Cell c(cell.row, cell.col);
    
    switch (dir)
    {
    case UP: c.row += 1; break;
    case DOWN: c.row -= 1; break;
    case LEFT: c.col -= 1; break;
    case RIGHT: c.col += 1; break;
    }
    
    // teleportation
    if (c.row == -1)     c.row = ROWS - 1;
    if (c.row == ROWS)   c.row = 0;
    if (c.col == -1)     c.col = COLS - 1;
    if (c.col == COLS)   c.col = 0;
    
    return c;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Snake *snake;
    
    switch (msg)
    {
    case WM_CREATE:
    {
        snake = new Snake();
        
        if(!SetTimer(hWnd, ID_TIMER, 80, NULL))
        {
            MessageBox(hWnd, "Could not set timer!", "Error", MB_OK | MB_ICONEXCLAMATION);
            PostQuitMessage(1);
        }
        
        break;
    }
    case WM_PAINT:
    {
        snake->Paint(hWnd);
        break;
    }
    case WM_TIMER:
    {
        snake->Update(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_KEYDOWN:
        snake->HandleKey(wParam);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        delete snake;
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
    RECT rcClient;
    UINT style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE; // no maximize box and resizing
    
    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = COLS * SCREEN_SCALE_FACTOR;
    rcClient.bottom = ROWS * SCREEN_SCALE_FACTOR;
    
    AdjustWindowRectEx(&rcClient, style, TRUE, 0);
    
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
                        TEXT("Snake"),
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
        return 1;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int) msg.wParam;
}