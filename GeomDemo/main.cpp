#include <windows.h>
#include "pch.h"
#include "GeomDemo.h"
#include "GeomView.h"

#include <cstring>

//=============================================================================
// GeomDemo 演示程序入口
//   - 默认：先输出全部演示结果，再弹出轻量 GDI 可视化窗口（Esc / 关闭窗口结束）
//   - 参数 --no-view：跳过可视化窗口，仅运行控制台演示
//=============================================================================
int main(int argc, char* argv[])
{
	// 让控制台以 UTF-8 显示中文
	SetConsoleOutputCP(CP_UTF8);

	bool bShowView = true;
	bool bSnapshot = false;
	const char* snapshotPath = "geom_demo_preview.bmp";
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--no-view") == 0)
		{
			bShowView = false;
		}
		else if (strcmp(argv[i], "--snapshot") == 0 && i + 1 < argc)
		{
			bSnapshot = true;
			snapshotPath = argv[++i];
			bShowView = false;
		}
	}

	GeomDemo demo;
	demo.RunAll();

	if (bSnapshot)
	{
		if (GeomView::Snapshot(demo.GetScene(), snapshotPath))
		{
			cout << "已离屏渲染场景到: " << snapshotPath << endl;
		}
		else
		{
			cout << "离屏渲染失败!" << endl;
		}
		return 0;
	}

	if (bShowView)
	{
		cout << endl
			<< "正在打开可视化窗口，关闭窗口（或按 Esc）后程序结束..." << endl;
		GeomView::Show(demo.GetScene());
		return 0;   // 窗口已关闭，直接退出
	}

	// 纯控制台模式：仅在交互式控制台下暂停，便于管道 / 自动化运行时不挂起
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwMode = 0;
	if (GetConsoleMode(hIn, &dwMode))
	{
		cout << endl << "按回车键退出...";
		cin.get();
	}
	return 0;
}
