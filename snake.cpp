/* g++ snake.cpp -o snake -lgdi32 -Wl,-subsystem,windows */

#include <deque>
#include <vector>

#include <cstdio>
#include <ctime>

#include <Windows.h>

#define ID_TIMER 1

constexpr int WND_WIDTH = 500;
constexpr int WND_HEIGHT = 500;

struct Cell
{
    int row, col;
    Cell(int row, int col) : row(row), col(col)
    {}
};

class Snake
{
public:
    Snake(int rows, int cols);
    ~Snake();
    void Update(HWND hWnd);
    void DrawBitmap(HDC hdc, RECT *rect, BITMAPINFO info);
    void HandleKey(WPARAM wParam);
    int GetScore();
private:
    enum Direction { UP = 0, DOWN, LEFT, RIGHT };
    enum CellType { EMPTY = 0, SNAKE, FOOD };
    
    std::vector<BYTE> pixels;
    std::deque<Cell> snake;
    
    int rows;
    int cols;
    int dir;
    
    void MakeFood();
    void SetCell(int row, int col, CellType type);
    CellType GetCellType(int row, int col);
    Cell Move(const Cell& cell);
};

Snake::Snake(int rows, int cols)
  : rows(rows), cols(cols)
{
    pixels.resize(rows * cols * 4);
    srand(static_cast<unsigned int>(time(NULL)));
    Snake::MakeFood();
    snake.push_back(Cell(rows / 2, cols / 2));
    dir = UP;
}

Snake::~Snake()
{}

void Snake::Update(HWND hWnd)
{
    snake.push_back(Snake::Move(snake.back()));
    
    Cell head = snake.back();
    bool eaten = false;
    
    if (Snake::GetCellType(head.row, head.col) == FOOD)
    {
        snake.push_back(Snake::Move(head));
        Cell newHead = snake.back();
        Snake::SetCell(newHead.row, newHead.col, SNAKE);
        eaten = true;
    }
    else if (Snake::GetCellType(head.row, head.col) == SNAKE)
    {
        char buff[200];
        snprintf(buff, sizeof(buff), "Score: %d", Snake::GetScore());
        KillTimer(hWnd, ID_TIMER);
        MessageBox(hWnd, TEXT(buff), "Game Over!", MB_OK | MB_ICONINFORMATION);
        DestroyWindow(hWnd);
    }
    
    Snake::SetCell(head.row, head.col, SNAKE);
    
    Cell tail = snake.front();
    snake.pop_front();
    Snake::SetCell(tail.row, tail.col, EMPTY);
    
    if (eaten) Snake::MakeFood();
}

void Snake::DrawBitmap(HDC hdc, RECT *rect, BITMAPINFO info)
{
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;
    
    StretchDIBits(hdc,
                  0,
                  0,
                  width,
                  height,
                  0,
                  0,
                  cols,
                  rows,
                  &pixels[0],
                  &info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
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

int Snake::GetScore()
{
    return snake.size();
}

void Snake::MakeFood()
{
    int row = rand() % rows;
    int col = rand() % cols;
    
    while (Snake::GetCellType(row, col) == SNAKE)
    {
        row = rand() % rows;
        col = rand() % cols;
    }
    
    Snake::SetCell(row, col, FOOD);
}

void Snake::SetCell(int row, int col, CellType type)
{
    int offset = row * cols * 4 + col * 4;
    
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
    int offset = row * rows * 4 + col * 4;
    
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
    if (c.row == -1)     c.row = rows - 1;
    if (c.row == rows)   c.row = 0;
    if (c.col == -1)     c.col = cols - 1;
    if (c.col == cols)   c.col = 0;
    
    return c;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static BITMAPINFO info;
    static Snake *snake;
    
    switch (msg)
    {
    case WM_CREATE:
    {
        int rows = 40;
        int cols = 40;
        
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = cols;
        info.bmiHeader.biHeight = rows;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = 0;
        info.bmiHeader.biXPelsPerMeter = 0;
        info.bmiHeader.biYPelsPerMeter = 0;
        info.bmiHeader.biClrUsed = 0;
        info.bmiHeader.biClrImportant = 0;
        
        snake = new Snake(rows, cols);
        
        if(!SetTimer(hWnd, ID_TIMER, 80, NULL))
        {
            MessageBox(hWnd, "Could not set timer!", "Error", MB_OK | MB_ICONEXCLAMATION);
            PostQuitMessage(1);
        }
        
        break;
    }
    case WM_PAINT:
    {
        RECT rcClient;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        GetClientRect(hWnd, &rcClient);
        snake->DrawBitmap(hdc, &rcClient, info);
        
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        RECT rcClient;
        HDC hdc = GetDC(hWnd);
        
        GetClientRect(hWnd, &rcClient);
        snake->Update(hWnd);
        snake->DrawBitmap(hdc, &rcClient, info);
        
        ReleaseDC(hWnd, hdc);
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
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, /* this window style prevents window resizing */
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        WND_WIDTH,
                        WND_HEIGHT,
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