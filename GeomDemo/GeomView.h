#pragma once

// 注意：必须先包含 windows.h，再包含 pch.h。
// pch.h 里有 using namespace std，会把 std::byte 引入全局命名空间，
// 与 Windows SDK 的 byte 类型冲突（C2872）。先处理 windows.h 可避免该问题。
#include <windows.h>
#include "pch.h"
#include "GeomScene.h"

//=============================================================================
// 轻量 GDI 可视化窗口：把 3D 场景用等轴测投影画到窗口，不做任何交互建模，
// 仅用于直观验证变换前后的几何关系。
//=============================================================================
class GeomView
{
public:
	GeomView();
	~GeomView();

	// 弹出窗口绘制场景，关闭窗口（或按 Esc）后返回
	static void Show(const GeomScene& scene);

	// 离屏渲染场景到 BMP 文件（无窗口预览），用于快速导出 / 调试
	static bool Snapshot(const GeomScene& scene, const char* bmpPath, int width = 940, int height = 700);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 绘制
	static void DrawScene(HDC hdc, const RECT& rc);
	static void CalcProjection(const GeomScene& scene, const RECT& rc);
	static void Project(const MPoint& p, double& sx, double& sy);
	static void DrawLine3D(HDC hdc, const MPoint& a, const MPoint& b, COLORREF color, int width = 2, int style = PS_SOLID);
	static void DrawPoint3D(HDC hdc, const MPoint& p, COLORREF color, int radius = 5);
	static void DrawPlane(HDC hdc, const MPlane& plane, double size);
	static void DrawBox(HDC hdc, const MBBox& box);
	static void DrawTri(HDC hdc, const MPoint tri[3], COLORREF color, int width, int style);

	// 投影状态（DrawScene 中先由 CalcProjection 求出）
	static double s_scale;
	static double s_cx;
	static double s_cy;
};
