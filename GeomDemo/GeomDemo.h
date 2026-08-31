#pragma once

#include "pch.h"
#include "MPoint.h"
#include "MVector.h"
#include "MLine.h"
#include "MPlane.h"
#include "MBBox.h"
#include "MMatrix.h"
#include "PointCompare.h"
#include "GeomScene.h"

//=============================================================================
// 全局辅助函数
//=============================================================================

// 两点的真实欧氏距离（库内 MPoint::DistanceToPoint() 已改为返回真实距离，
// 此处保留一个便捷的独立实现，避免与库产生依赖）。
double Dist(const MPoint& a, const MPoint& b);

// 打印辅助
void PrintSection(const char* title);          // 输出章节分隔
void PrintPoint(const char* name, const MPoint& p);
void PrintVector(const char* name, const MVector& v);
void PrintMatrix(const char* name, const MMatrix& m);
std::string PtStr(const MPoint& p);            // "(x, y, z)"
std::string VecStr(const MVector& v);

//=============================================================================
// 演示程序类：按顺序演示全部核心功能
//   1 建立几何对象
//   2 计算空间关系
//   3 求解交点
//   4 判断空间范围
//   5 进行矩阵变换
//   6 验证变换后的结果（可选：轻量 GDI 可视化）
//=============================================================================
class GeomDemo
{
public:
	GeomDemo();
	virtual ~GeomDemo();

	void RunAll();                    // 依次运行全部演示
	const GeomScene& GetScene() const; // 供 GeomView 可视化使用

private:
	void DemoBuildObjects();  // 1 建立几何对象
	void DemoRelation();      // 2 计算空间关系
	void DemoIntersect();     // 3 求解交点
	void DemoRange();         // 4 判断空间范围
	void DemoTransform();     // 5 进行矩阵变换
	void DemoVerify();        // 6 验证变换后的结果

	GeomScene* m_pScene;      // 供可视化使用的场景数据
};
