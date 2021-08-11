/* g++ paint.cpp -o paint -lgdi32 -Wl,-subsystem,windows */
// https://gist.github.com/rfaruga/0dd5d8409a8e94eb1b6ce0daa5c7fd41

#include <Windows.h>
#include <wchar.h> 

PAINTSTRUCT lpPaint;
int width, height;
RECT lpRect;
MSG msg;
BOOL LButtonDOWN = false;


// Shape

int Shape, Color;
BOOL FullColor;
POINT pos1;
POINT pos2;

COLORREF colors[16] = {
	{ RGB(0, 0, 0) }, // 1. czarny
	{ RGB(150, 75, 0) }, // 2. brązowy
	{ RGB(0, 128, 0) }, // 3. zielony
	{ RGB(128, 128, 0) }, // 4. oliwkowy
	{ RGB(0, 0, 128) }, // 5. granatowy
	{ RGB(184, 3, 255) }, // 6. fiolet
	{ RGB(0, 128, 128) }, // 7. morski
	{ RGB(128, 128, 128) }, // 8. szary
	{ RGB(192, 192, 192) }, // 9. srebrny
	{ RGB(255, 0, 0) }, // 10. czerwony
	{ RGB(0, 255, 0) }, // 11. limonka
	{ RGB(255, 255, 0) }, // 12. żółty
	{ RGB(0, 0, 255) }, // 13. niebieski
	{ RGB(255, 204, 221) }, // 14. różowy
	{ RGB(0, 255, 255) }, // 15. cyjan
	{ RGB(255, 255, 255) } // 16. biały
};

HPEN pen[16] = 
{
	{ CreatePen(PS_SOLID, 1, colors[0]) },
	{ CreatePen(PS_SOLID, 1, colors[1]) },
	{ CreatePen(PS_SOLID, 1, colors[2]) },
	{ CreatePen(PS_SOLID, 1, colors[3]) },
	{ CreatePen(PS_SOLID, 1, colors[4]) },
	{ CreatePen(PS_SOLID, 1, colors[5]) },
	{ CreatePen(PS_SOLID, 1, colors[6]) },
	{ CreatePen(PS_SOLID, 1, colors[7]) },
	{ CreatePen(PS_SOLID, 1, colors[8]) },
	{ CreatePen(PS_SOLID, 1, colors[9]) },
	{ CreatePen(PS_SOLID, 1, colors[10]) },
	{ CreatePen(PS_SOLID, 1, colors[11]) },	
	{ CreatePen(PS_SOLID, 1, colors[12]) },
	{ CreatePen(PS_SOLID, 1, colors[13]) },
	{ CreatePen(PS_SOLID, 1, colors[14]) },
	{ CreatePen(PS_SOLID, 1, colors[15]) },
};

HBRUSH brush[16] =
{
	{ CreateSolidBrush(colors[0]) },
	{ CreateSolidBrush(colors[1]) },
	{ CreateSolidBrush(colors[2]) },
	{ CreateSolidBrush(colors[3]) },
	{ CreateSolidBrush(colors[4]) },
	{ CreateSolidBrush(colors[5]) },
	{ CreateSolidBrush(colors[6]) },
	{ CreateSolidBrush(colors[7]) },
	{ CreateSolidBrush(colors[8]) },
	{ CreateSolidBrush(colors[9]) },
	{ CreateSolidBrush(colors[10]) },
	{ CreateSolidBrush(colors[11]) },
	{ CreateSolidBrush(colors[12]) },
	{ CreateSolidBrush(colors[13]) },
	{ CreateSolidBrush(colors[14]) },
	{ CreateSolidBrush(colors[15]) },
};

LRESULT CALLBACK ProceduraOkna(HWND, UINT, WPARAM, LPARAM);

int ChooseColorDraw(POINT*);
int ChooseShape(POINT*);
void Draw(HWND, int);

int WINAPI WinMain(HINSTANCE hInstance,  
                   HINSTANCE hPrevInstance,  
                   LPSTR lpCmdLine,  
                   int nCmdShow)
{
	WNDCLASSEX window;
	window.cbSize		  = sizeof(WNDCLASSEX);  
	window.style          = CS_VREDRAW | CS_HREDRAW;
	window.lpfnWndProc    = ProceduraOkna;  
	window.cbClsExtra     = 0;  
	window.cbWndExtra     = 0;  
	window.hInstance      = hInstance;  
	window.hIcon          = LoadIcon(NULL, IDI_APPLICATION);  
	window.hCursor        = LoadCursor(NULL, IDC_ARROW);  
	window.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);  
	window.lpszMenuName   = 0;  
	window.lpszClassName  = TEXT("Standardowe Okno");  
	window.hIconSm        = LoadIcon(0, (LPCTSTR)IDI_APPLICATION);  
	
	RegisterClassEx(&window);

	HWND UchwytOkna = 0;
	UchwytOkna = CreateWindowEx(
			0,
			TEXT("Standardowe Okno"),
			TEXT("Paint"),
			WS_OVERLAPPEDWINDOW,
			400,
			300,
			1000,
			530,
			0,
			0,
			hInstance,
			0);

	ShowWindow(UchwytOkna, SW_NORMAL);
	UpdateWindow(UchwytOkna);

	while(GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(TEXT("StandardoweOkno"), NULL);

	return (int)msg.wParam;
}

LRESULT CALLBACK ProceduraOkna(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hwnd, &lpPaint);
			
			UpdateWindow(hwnd);

			// PAINT BACKGROUND
			GetClientRect(hwnd, &lpRect);
			SelectObject(hdc, pen[15]);
			Rectangle(hdc, lpRect.left, lpRect.top, lpRect.right-100, lpRect.bottom);

			// MENU BACKGROUND
			width = lpRect.right;
			height = lpRect.bottom;
			HBRUSH menu = CreateSolidBrush(RGB(216, 216, 216));
			SelectObject(hdc, menu);
			Rectangle(hdc, width - 100, 0, width, height);
			DeleteObject(menu);

			// MENU COLOR
			for (int i = 0; i < 16; i++)
			{
				SelectObject(hdc, pen[i]);
				SelectObject(hdc, brush[i++]);
				Rectangle(hdc, width - 85, 20*i + 25, width - 60, 20*i);
				SelectObject(hdc, pen[i]);
				SelectObject(hdc, brush[i]);
				Rectangle(hdc, width - 40, 20*i + 25, width - 15, 20*i);
			}

			// MENU SHAPE
			for (int i = 0; i < 4; i++)
			{
				SelectObject(hdc, pen[0]);
				SelectObject(hdc, brush[15]);
				i++;
				Rectangle(hdc, width - 85, 20 * i + 400, width - 60, 20 * i + 375);
				SelectObject(hdc, pen[0]);
				SelectObject(hdc, brush[15]);
				Rectangle(hdc, width - 40, 20 * i + 400, width - 15, 20 * i + 375);
			}

			SelectObject(hdc, brush[0]);
			Rectangle(hdc, width - 80, 415, width - 65, 400);

			SelectObject(hdc, brush[15]);
			POINT triangle1[3] = { {width - 35, 400}, {width - 20, 400}, {width - 27.5, 415 } };
			Polygon(hdc, triangle1, 3);
			
			SelectObject(hdc, brush[0]);
			SelectObject(hdc, pen[0]);
			POINT triangle2[3] = { { width - 35, 440 },{ width - 20, 440 },{ width - 27.5, 455 } };
			Polygon(hdc, triangle2, 3);

			SelectObject(hdc, brush[0]);
			Ellipse(hdc, width - 80, 455, width - 65, 440);

			EndPaint(hwnd, &lpPaint);
		}
		break;

		case WM_LBUTTONDOWN:
		{
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hwnd, &point);
			
			if (point.x > width - 85 && point.y < 325)
			{
				Color = ChooseColorDraw(&point);
				FullColor = true;
			}

			if (point.x > width - 85 && point.y > 395)
			{
				Shape = ChooseShape(&point);
			}
			if (!LButtonDOWN && point.x < width - 100)
			{
				LButtonDOWN = true;
				GetCursorPos(&pos1);
				ScreenToClient(hwnd, &pos1);
			}
		}
		break;

		case WM_LBUTTONUP:
		{	
			if (LButtonDOWN)
			{
				HDC hdc = GetDC(hwnd);
				GetCursorPos(&pos2);
				ScreenToClient(hwnd, &pos2);

				if ((pos1.x < width - 100) && (pos2.x < width - 100)) Draw(hwnd, Shape);
				ReleaseDC(hwnd, hdc);
				LButtonDOWN = false;
			}
		}
		break;

		case WM_KEYDOWN:
		{
			switch ((int)wParam)
			{
				case VK_ESCAPE:
				{
					HDC hdc = GetDC(hwnd);
					GetClientRect(hwnd, &lpRect);
					SelectObject(hdc, pen[15]);
					Rectangle(hdc, lpRect.left, lpRect.top, lpRect.right - 100, lpRect.bottom);
					ReleaseDC(hwnd, hdc);
				}
			break;
			}
		}
		break;

		case WM_RBUTTONDOWN:
		{
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hwnd, &point);

			if (point.x > width - 85 && point.y < 325)
			{
				Color = ChooseColorDraw(&point);
				FullColor = false;
			}
		}
		break;

		case WM_MOUSEMOVE:
		{
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hwnd, &point);
			TCHAR buff[30];
			//swprintf_s(buff, 30, TEXT("Paint (x: %d, y: %d)"), point.x, point.y);
			SetWindowText(hwnd, buff);
		}
		break;

		case WM_CLOSE:
		{
			SetWindowText(hwnd, TEXT("Paint"));
			if (MessageBox(hwnd, TEXT("Zakończyć prace?"), TEXT("Paint"), MB_OKCANCEL) == IDOK)
			{
				for (int i = 0; i < 16; i++)
				{
					DeleteObject(pen[i]);
					DeleteObject(brush[i]);
				}
				DestroyWindow(hwnd);
			}
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void Draw(HWND hwnd, int Shape)
{
	HDC hdc = GetDC(hwnd);
	if (FullColor)
	{
		SelectObject(hdc, brush[Color]);
		SelectObject(hdc, pen[Color]);
	}
	else
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		SelectObject(hdc, pen[Color]);
	}

	switch (Shape)
	{
	case 1:
	{
		Rectangle(hdc, pos1.x, pos1.y, pos2.x, pos2.y);
	}
		break;
	case 2:
	{
		Ellipse(hdc, pos1.x, pos1.y, pos2.x, pos2.y);
	}
		break;
	case 3:
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		POINT point[3] = { { pos1.x, pos1.y }, { pos2.x, pos2.y }, { pos1.x, pos2.y } };
		Polygon(hdc, point, 3);
	}
		break;
	case 4:
	{
		POINT point[3] = { { pos1.x, pos1.y }, { pos2.x, pos2.y }, { pos2.x, pos1.y } };
		Polygon(hdc, point, 3);
	}
		break;
	default:
		return;
	}
	ReleaseDC(hwnd, hdc);
}

int ChooseShape(POINT *Point)
{
	int ActiveShape = Shape;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 395) && (Point->y <= 420)) ActiveShape = 1;
	else if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 435) && (Point->y <= 460)) ActiveShape = 2;
	else if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 395) && (Point->y <= 420)) ActiveShape = 3;
	else if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 435) && (Point->y <= 460)) ActiveShape = 4;
	return ActiveShape;
}

int ChooseColorDraw(POINT *Point)
{
	int Color = 0;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 20) && (Point->y <= 45)) Color = 0;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 60) && (Point->y <= 85)) Color = 2;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 100) && (Point->y <= 125)) Color = 4;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 140) && (Point->y <= 165)) Color = 6;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 180) && (Point->y <= 205)) Color = 8;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 220) && (Point->y <= 245)) Color = 10;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 260) && (Point->y <= 285)) Color = 12;
	if ((Point->x >= (width - 85)) && (Point->x <= (width - 60)) && (Point->y >= 300) && (Point->y <= 325)) Color = 14;

	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 20) && (Point->y <= 45)) Color = 1;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 60) && (Point->y <= 85)) Color = 3;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 100) && (Point->y <= 125)) Color = 5;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 140) && (Point->y <= 165)) Color = 7;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 180) && (Point->y <= 205)) Color = 9;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 220) && (Point->y <= 245)) Color = 11;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 260) && (Point->y <= 285)) Color = 13;
	if ((Point->x >= (width - 40)) && (Point->x <= (width - 15)) && (Point->y >= 300) && (Point->y <= 325)) Color = 15;
	return Color;
}