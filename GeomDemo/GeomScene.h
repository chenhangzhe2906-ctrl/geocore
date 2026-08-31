#pragma once

#include "pch.h"
#include "MPoint.h"
#include "MLine.h"
#include "MPlane.h"
#include "MBBox.h"

//=============================================================================
// 可视化场景：由 GeomDemo 运行过程中填充，GeomView 负责将其绘制到窗口。
// 本头文件保持纯 C++，不包含 windows.h，供 GeomDemo / GeomView 共同使用。
//=============================================================================
struct GeomScene
{
	GeomScene();

	// 直线求交演示
	MPoint line1Start, line1End;
	MPoint line2Start, line2End;
	MPoint lineIntersection;
	bool   bShowLines;

	// 线段求交演示
	MPoint seg1Start, seg1End;
	MPoint seg2Start, seg2End;
	MPoint segIntersection;
	bool   bShowSegs;

	// 平面与直线求交演示
	MPlane  plane;
	MPoint  linePlaneStart, linePlaneEnd;
	MPoint  planeHit;
	bool    bShowPlane;

	// 包围盒与空间范围判断
	MBBox   box;
	MPoint  boxPtIn, boxPtOut;
	bool    bShowBox;

	// 三角形旋转变换验证（原始 vs 变换后）
	MPoint  triOrig[3];
	MPoint  triTrans[3];
	bool    bShowTri;
};
