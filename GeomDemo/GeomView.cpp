#include <windows.h>
#include "pch.h"
#include "GeomView.h"

#include <cmath>
#include <vector>
#include <cstdio>

//=============================================================================
// 投影状态
//=============================================================================
double GeomView::s_scale = 40.0;
double GeomView::s_cx = 0.0;
double GeomView::s_cy = 0.0;

static const GeomScene* g_scene = nullptr;
static const wchar_t* g_kClassName = L"GeomDemoView";

//=============================================================================
// GeomScene
//=============================================================================

// 离屏渲染：把场景画到内存 DIB，再保存为 24 位 BMP 文件
bool GeomView::Snapshot(const GeomScene& scene, const char* bmpPath, int width, int height)
{
	HDC hdcScreen = GetDC(NULL);
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;          // 正数：自底向上存储
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;

	void* bits = NULL;
	HBITMAP dib = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
	HDC mem = CreateCompatibleDC(hdcScreen);
	HGDIOBJ old = SelectObject(mem, dib);

	RECT rc = { 0, 0, width, height };
	g_scene = &scene;
	DrawScene(mem, rc);
	g_scene = NULL;

	// 写出 BMP
	FILE* f = NULL;
	fopen_s(&f, bmpPath, "wb");
	bool bOk = (f != NULL);
	if (bOk)
	{
		int rowSize = ((width * 3 + 3) / 4) * 4;
		BITMAPFILEHEADER fh;
		memset(&fh, 0, sizeof(fh));
		fh.bfType = 0x4D42;   // 'BM'
		fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fh.bfSize = fh.bfOffBits + (DWORD)rowSize * height;
		fwrite(&fh, sizeof(fh), 1, f);

		BITMAPINFOHEADER ih = bi.bmiHeader;
		ih.biSizeImage = rowSize * height;
		fwrite(&ih, sizeof(ih), 1, f);
		fwrite(bits, rowSize * height, 1, f);
		fclose(f);
	}

	SelectObject(mem, old);
	DeleteDC(mem);
	DeleteObject(dib);
	ReleaseDC(NULL, hdcScreen);
	return bOk;
}

GeomScene::GeomScene()
{
	bShowLines = false;
	bShowSegs = false;
	bShowPlane = false;
	bShowBox = false;
	bShowTri = false;
}

//=============================================================================
// 投影：用一个固定的等轴测视角把 3D 点投影到 2D（x -> 右，z -> 上）
//=============================================================================

static MMatrix BuildViewMatrix()
{
	MVector ax(1.0, 0.0, 0.0);
	MVector ay(0.0, 1.0, 0.0);
	MMatrix rx(3, 3);
	rx = MMatrix::s_GetRotationMatrix(ax, 0.55);    // 先俯视 ~31°
	MMatrix ry(3, 3);
	ry = MMatrix::s_GetRotationMatrix(ay, -0.65);   // 再水平转 ~-37°
	return rx * ry;                                  // 复合视角矩阵
}

void GeomView::Project(const MPoint& p, double& sx, double& sy)
{
	// C++17 保证返回值直接构造，不会触发库中浅拷贝的拷贝构造
	static const MMatrix s_view = BuildViewMatrix();
	MPoint q = s_view * p;
	sx = q[0];
	sy = q[2];
}

void GeomView::CalcProjection(const GeomScene& scene, const RECT& rc)
{
	// 收集场景中所有需要显示的点，求出投影后的包围范围
	vector<MPoint> pts;
	if (scene.bShowLines)
	{
		pts.push_back(scene.line1Start);
		pts.push_back(scene.line1End);
		pts.push_back(scene.line2Start);
		pts.push_back(scene.line2End);
		pts.push_back(scene.lineIntersection);
	}
	if (scene.bShowSegs)
	{
		pts.push_back(scene.seg1Start);
		pts.push_back(scene.seg1End);
		pts.push_back(scene.seg2Start);
		pts.push_back(scene.seg2End);
		pts.push_back(scene.segIntersection);
	}
	if (scene.bShowPlane)
	{
		pts.push_back(scene.plane.GetArbitraryPoint());
		pts.push_back(scene.linePlaneStart);
		pts.push_back(scene.linePlaneEnd);
		pts.push_back(scene.planeHit);
	}
	if (scene.bShowBox)
	{
		MPoint corners[8];
		scene.box.Get8Corners(corners);
		for (int i = 0; i < 8; i++)
		{
			pts.push_back(corners[i]);
		}
		pts.push_back(scene.boxPtIn);
		pts.push_back(scene.boxPtOut);
	}
	if (scene.bShowTri)
	{
		for (int i = 0; i < 3; i++)
		{
			pts.push_back(scene.triOrig[i]);
			pts.push_back(scene.triTrans[i]);
		}
	}
	// 至少给一个默认点
	if (pts.empty())
	{
		pts.push_back(MPoint(0.0, 0.0, 0.0));
	}

	double minX = 1e30, maxX = -1e30, minY = 1e30, maxY = -1e30;
	for (size_t i = 0; i < pts.size(); i++)
	{
		double px = 0.0, py = 0.0;
		Project(pts[i], px, py);
		minX = (minX < px) ? minX : px;
		maxX = (maxX > px) ? maxX : px;
		minY = (minY < py) ? minY : py;
		maxY = (maxY > py) ? maxY : py;
	}

	double spanX = maxX - minX;
	double spanY = maxY - minY;
	if (spanX < 1e-9) spanX = 1e-9;
	if (spanY < 1e-9) spanY = 1e-9;

	int rcW = rc.right - rc.left;
	int rcH = rc.bottom - rc.top;
	// 按 X、Y 两个方向分别求缩放，取较小者，保证整个场景都能落进窗口
	double scaleX = (rcW * 0.8) / spanX;
	double scaleY = (rcH * 0.8) / spanY;
	s_scale = (scaleX < scaleY) ? scaleX : scaleY;

	s_cx = (rc.left + rc.right) / 2.0 - (minX + maxX) / 2.0 * s_scale;
	s_cy = (rc.top + rc.bottom) / 2.0 + (minY + maxY) / 2.0 * s_scale;   // 屏幕 y 向下，取反
}

//=============================================================================
// 绘制图元
//=============================================================================

void GeomView::DrawLine3D(HDC hdc, const MPoint& a, const MPoint& b, COLORREF color, int width, int style)
{
	HPEN pen = CreatePen(style, width, color);
	HGDIOBJ old = SelectObject(hdc, pen);

	double ax = 0.0, ay = 0.0, bx = 0.0, by = 0.0;
	Project(a, ax, ay);
	Project(b, bx, by);
	int x1 = (int)(s_cx + ax * s_scale);
	int y1 = (int)(s_cy - ay * s_scale);
	int x2 = (int)(s_cx + bx * s_scale);
	int y2 = (int)(s_cy - by * s_scale);

	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);

	SelectObject(hdc, old);
	DeleteObject(pen);
}

void GeomView::DrawPoint3D(HDC hdc, const MPoint& p, COLORREF color, int radius)
{
	double px = 0.0, py = 0.0;
	Project(p, px, py);
	int x = (int)(s_cx + px * s_scale);
	int y = (int)(s_cy - py * s_scale);

	HBRUSH br = CreateSolidBrush(color);
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HGDIOBJ ob = SelectObject(hdc, br);
	HGDIOBJ op = SelectObject(hdc, pen);

	Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

	SelectObject(hdc, ob);
	SelectObject(hdc, op);
	DeleteObject(br);
	DeleteObject(pen);
}

void GeomView::DrawPlane(HDC hdc, const MPlane& plane, double size)
{
	// 以平面上任一点为中心，沿两个面内方向画一个平行四边形表示平面
	MVector n = plane.Normal();
	MVector ref(0.0, 0.0, 1.0);
	if (n.IsParallelTo(ref))
	{
		ref.Set(1.0, 0.0, 0.0);
	}
	MVector u = n * ref;          // 面内方向 1（叉积）
	u.Normalize();
	MVector v = n * u;            // 面内方向 2（与 n、u 都垂直）
	v.Normalize();

	MPoint c = plane.GetArbitraryPoint();
	double half = size / 2.0;
	// 注意：库未定义“MPoint - MVector”，一律用 MPoint + (-MVector) 表达
	MVector du = u * half;
	MVector dv = v * half;
	MPoint corners[4];
	corners[0] = c + du + dv;
	corners[1] = c + du + (-dv);
	corners[2] = c + (-du) + (-dv);
	corners[3] = c + (-du) + dv;

	double p2d[4][2];
	for (int i = 0; i < 4; i++)
	{
		double px = 0.0, py = 0.0;
		Project(corners[i], px, py);
		p2d[i][0] = s_cx + px * s_scale;
		p2d[i][1] = s_cy - py * s_scale;
	}

	// 半透明填充（浅蓝）
	HBRUSH br = CreateSolidBrush(RGB(205, 225, 245));
	HGDIOBJ ob = SelectObject(hdc, br);
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(90, 130, 180));
	HGDIOBJ op = SelectObject(hdc, pen);

	POINT pt[4];
	for (int i = 0; i < 4; i++)
	{
		pt[i].x = (LONG)p2d[i][0];
		pt[i].y = (LONG)p2d[i][1];
	}
	Polygon(hdc, pt, 4);

	SelectObject(hdc, op);
	SelectObject(hdc, ob);
	DeleteObject(pen);
	DeleteObject(br);
}

void GeomView::DrawBox(HDC hdc, const MBBox& box)
{
	MPoint c[8];
	box.Get8Corners(c);
	int edges[12][2] = {
		{0,1},{1,2},{2,3},{3,0},   // 底面
		{4,5},{5,6},{6,7},{7,4},   // 顶面
		{0,4},{1,5},{2,6},{3,7}    // 竖棱
	};
	for (int i = 0; i < 12; i++)
	{
		DrawLine3D(hdc, c[edges[i][0]], c[edges[i][1]], RGB(220, 140, 40), 1, PS_SOLID);
	}
}

void GeomView::DrawTri(HDC hdc, const MPoint tri[3], COLORREF color, int width, int style)
{
	for (int i = 0; i < 3; i++)
	{
		DrawLine3D(hdc, tri[i], tri[(i + 1) % 3], color, width, style);
	}
}

//=============================================================================
// 窗口绘制与消息循环
//=============================================================================

void GeomView::DrawScene(HDC hdc, const RECT& rc)
{
	if (g_scene == NULL)
	{
		return;
	}
	CalcProjection(*g_scene, rc);

	// 背景
	HBRUSH bg = CreateSolidBrush(RGB(250, 250, 253));
	FillRect(hdc, &rc, bg);
	DeleteObject(bg);

	// 参考网格
	HPEN grid = CreatePen(PS_SOLID, 1, RGB(228, 228, 235));
	HGDIOBJ og = SelectObject(hdc, grid);
	for (int x = 30; x < rc.right; x += 30)
	{
		MoveToEx(hdc, x, 0, NULL);
		LineTo(hdc, x, rc.bottom);
	}
	for (int y = 30; y < rc.bottom; y += 30)
	{
		MoveToEx(hdc, 0, y, NULL);
		LineTo(hdc, rc.right, y);
	}
	SelectObject(hdc, og);
	DeleteObject(grid);

	// 绘制顺序：先面，再线，最后关键点
	if (g_scene->bShowPlane)
	{
		DrawPlane(hdc, g_scene->plane, 6.0);
	}
	if (g_scene->bShowBox)
	{
		DrawBox(hdc, g_scene->box);
	}
	if (g_scene->bShowTri)
	{
		DrawTri(hdc, g_scene->triOrig, RGB(70, 120, 220), 2, PS_DOT);
		DrawTri(hdc, g_scene->triTrans, RGB(200, 60, 60), 3, PS_SOLID);
	}

	if (g_scene->bShowLines)
	{
		DrawLine3D(hdc, g_scene->line1Start, g_scene->line1End, RGB(200, 60, 60), 2, PS_SOLID);
		DrawLine3D(hdc, g_scene->line2Start, g_scene->line2End, RGB(60, 160, 60), 2, PS_SOLID);
		DrawPoint3D(hdc, g_scene->lineIntersection, RGB(0, 0, 0), 6);
	}
	if (g_scene->bShowSegs)
	{
		DrawLine3D(hdc, g_scene->seg1Start, g_scene->seg1End, RGB(180, 100, 40), 2, PS_DASH);
		DrawLine3D(hdc, g_scene->seg2Start, g_scene->seg2End, RGB(40, 100, 180), 2, PS_DASH);
		DrawPoint3D(hdc, g_scene->segIntersection, RGB(0, 0, 0), 6);
	}
	if (g_scene->bShowPlane)
	{
		DrawLine3D(hdc, g_scene->linePlaneStart, g_scene->linePlaneEnd, RGB(0, 80, 200), 2, PS_SOLID);
		DrawPoint3D(hdc, g_scene->planeHit, RGB(240, 180, 0), 7);
	}
	if (g_scene->bShowBox)
	{
		DrawPoint3D(hdc, g_scene->boxPtIn, RGB(0, 150, 0), 5);
		DrawPoint3D(hdc, g_scene->boxPtOut, RGB(200, 0, 0), 5);
	}
}

LRESULT CALLBACK GeomView::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);
		DrawScene(hdc, rc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;   // 由 WM_PAINT 统一绘制，避免闪烁
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void GeomView::Show(const GeomScene& scene)
{
	g_scene = &scene;

	HINSTANCE hInst = GetModuleHandleW(NULL);
	WNDCLASSEXW wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = g_kClassName;
	RegisterClassExW(&wc);   // 重复注册会失败，忽略即可

	HWND hWnd = CreateWindowExW(0, g_kClassName, L"GeomDemo —— 几何基础库演示可视化",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 960, 760,
		NULL, NULL, hInst, NULL);
	if (hWnd == NULL)
	{
		g_scene = NULL;
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	g_scene = NULL;
}

//=============================================================================

GeomView::GeomView()
{
}

GeomView::~GeomView()
{
}
