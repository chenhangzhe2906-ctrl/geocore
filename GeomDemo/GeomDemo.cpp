#include "pch.h"
#include "GeomDemo.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstdio>

//=============================================================================
// 全局辅助函数
//=============================================================================

double Dist(const MPoint& a, const MPoint& b)
{
	return sqrt((a[0] - b[0]) * (a[0] - b[0]) +
		(a[1] - b[1]) * (a[1] - b[1]) +
		(a[2] - b[2]) * (a[2] - b[2]));
}

void PrintSection(const char* title)
{
	cout << endl
		<< "=====================================================================" << endl
		<< "  " << title << endl
		<< "=====================================================================" << endl;
}

std::string PtStr(const MPoint& p)
{
	char buf[128];
	sprintf_s(buf, "(%.6f, %.6f, %.6f)", p[0], p[1], p[2]);
	return std::string(buf);
}

std::string VecStr(const MVector& v)
{
	char buf[128];
	sprintf_s(buf, "(%.6f, %.6f, %.6f)", v[0], v[1], v[2]);
	return std::string(buf);
}

void PrintPoint(const char* name, const MPoint& p)
{
	cout << "    " << name << " = " << PtStr(p) << endl;
}

void PrintVector(const char* name, const MVector& v)
{
	cout << "    " << name << " = " << VecStr(v) << endl;
}

void PrintMatrix(const char* name, const MMatrix& m)
{
	cout << "    " << name << " (" << m.GetRow() << "x" << m.GetCol() << "):" << endl;
	cout << fixed << setprecision(5);
	for (int i = 0; i < m.GetRow(); i++)
	{
		cout << "      ";
		for (int j = 0; j < m.GetCol(); j++)
		{
			cout << setw(10) << m[i][j] << " ";
		}
		cout << endl;
	}
	cout << resetiosflags(ios::fixed);
}

// 输出一项检查结果
static void Check(bool bOk, const char* desc)
{
	cout << "    " << (bOk ? "[通过]" : "[失败]  !!") << "  " << desc << endl;
}

//=============================================================================
// GeomDemo
//=============================================================================

GeomDemo::GeomDemo()
	: m_pScene(new GeomScene())
{
}

GeomDemo::~GeomDemo()
{
	delete m_pScene;
}

void GeomDemo::RunAll()
{
	DemoBuildObjects();
	DemoRelation();
	DemoIntersect();
	DemoRange();
	DemoTransform();
	DemoVerify();

	cout << endl << "===== 全部演示结束 =====" << endl;
}

const GeomScene& GeomDemo::GetScene() const
{
	return *m_pScene;
}

//-----------------------------------------------------------------------------
// 1 建立几何对象
//-----------------------------------------------------------------------------
void GeomDemo::DemoBuildObjects()
{
	PrintSection("第 1 步：建立几何对象");

	// 点
	MPoint pA(1.0, 2.0, 3.0);
	MPoint pB(4.0, 6.0, 8.0);
	PrintPoint("点 pA", pA);
	PrintPoint("点 pB", pB);

	// 向量
	MVector vX(1.0, 0.0, 0.0);
	MVector vY(0.0, 1.0, 0.0);
	MVector vZ(0.0, 0.0, 1.0);
	PrintVector("向量 vX", vX);
	PrintVector("向量 vY", vY);

	// 直线（库内构造函数会自动对方向向量做归一化）
	MLine line1(pA, vX);
	MLine line2(pB, vY);
	PrintPoint("line1 经过的点", line1.Point());
	PrintVector("line1 方向(已归一化)", line1.Direction());
	PrintPoint("line2 经过的点", line2.Point());
	PrintVector("line2 方向(已归一化)", line2.Direction());

	// 线段
	MLineSeg seg(pA, pB);
	PrintPoint("线段 seg 起点", seg.GetStartPoint());
	PrintPoint("线段 seg 终点", seg.GetEndPoint());

	// 平面：过原点、法向为 Z 轴 -> 平面方程 z = 0
	MVector nrm(0.0, 0.0, 1.0);
	MPoint origin(0.0, 0.0, 0.0);
	MPlane plane(nrm, origin);
	PrintVector("平面法向", plane.Normal());
	cout << "    平面常数项 d = " << plane.GetCoefD() << "   (平面方程: z = 0)" << endl;

	// 包围盒
	MBBox box;
	box.Set(MPoint(0.0, 0.0, 0.0), MPoint(10.0, 10.0, 10.0));
	PrintPoint("包围盒最小值点", box.GetMinPnt());
	PrintPoint("包围盒最大值点", box.GetMaxPnt());

	// 供可视化使用：两条求交直线
	m_pScene->line1Start.Set(0.0, 0.0, 0.0);
	m_pScene->line1End.Set(2.0, 2.0, 0.0);
	m_pScene->line2Start.Set(0.0, 2.0, 0.0);
	m_pScene->line2End.Set(2.0, 0.0, 0.0);
	m_pScene->bShowLines = true;
}

//-----------------------------------------------------------------------------
// 2 计算空间关系
//-----------------------------------------------------------------------------
void GeomDemo::DemoRelation()
{
	PrintSection("第 2 步：计算空间关系");

	// 点到直线的距离与投影
	MLine line1(MPoint(0.0, 0.0, 0.0), MVector(1.0, 1.0, 0.0));
	MPoint pC(1.0, 1.0, 5.0);
	MPoint proj = line1.ProjectPoint(pC);
	PrintPoint("点 pC", pC);
	PrintPoint("pC 在 line1 上的投影点", proj);
	cout << "    库返回的 点->直线 距离 = " << line1.DistanceToPoint(pC) << endl;
	Check(fabs(line1.DistanceToPoint(pC) - 5.0) < 1e-9, "pC 到直线 line1 的距离应为 5");

	// 点到线段的距离与投影（投影参数 t 输出）
	MLineSeg seg(MPoint(0.0, 0.0, 0.0), MPoint(4.0, 6.0, 8.0));
	MPoint pE(1.0, 1.0, 1.0);
	double t = 0.0;
	MPoint projSeg = seg.ProjectPoint(pE, &t);
	PrintPoint("pE 在线段 seg 上的投影点", projSeg);
	cout << "    投影参数 t = " << t << endl;
	cout << "    pE 到线段 seg 的距离 = " << seg.DistanceToPoint(pE) << endl;
	Check(seg.IsContain(projSeg), "投影点应落在线段上");

	// 点是否在线段上
	Check(seg.IsContain(MPoint(2.0, 3.0, 4.0)), "线段中点 (2,3,4) 应在线段上");
	Check(!seg.IsContain(MPoint(10.0, 10.0, 10.0)), "(10,10,10) 不应在线段上");

	// 点到平面的有符号距离
	MVector nrm(0.0, 0.0, 1.0);
	MPoint origin(0.0, 0.0, 0.0);
	MPlane plane(nrm, origin);                 // z = 0
	cout << "    (5,0,4)  到平面 z=0 的有符号距离 = " << plane.DistanceToPoint(MPoint(5.0, 0.0, 4.0), true) << endl;
	cout << "    (5,0,-3) 到平面 z=0 的有符号距离 = " << plane.DistanceToPoint(MPoint(5.0, 0.0, -3.0), true) << endl;

	// 向量的夹角 / 平行判断
	MVector vA(1.0, 0.0, 0.0);
	MVector vB(0.0, 1.0, 0.0);
	double ang = vA.AngleWith(vB);
	cout << "    vA 与 vB 的夹角 = " << ang << " 弧度 ≈ " << ang * 180.0 / 3.14159265358979 << " 度" << endl;
	Check(vA.IsParallelTo(MVector(2.0, 0.0, 0.0)), "vA 应平行于 (2,0,0)");
	Check(!vA.IsParallelTo(vB), "vA 不应平行于 vB");

	// 点排序（PointCompare）
	vector<MPoint> pts;
	pts.push_back(MPoint(3.0, 0.0, 0.0));
	pts.push_back(MPoint(1.0, 0.0, 0.0));
	pts.push_back(MPoint(2.0, 0.0, 0.0));
	sort(pts.begin(), pts.end(), PointCompare());
	cout << "    PointCompare 排序结果: ";
	for (size_t i = 0; i < pts.size(); i++)
	{
		cout << "(" << pts[i][0] << "," << pts[i][1] << "," << pts[i][2] << ") ";
	}
	cout << endl;
}

//-----------------------------------------------------------------------------
// 3 求解交点
//-----------------------------------------------------------------------------
void GeomDemo::DemoIntersect()
{
	PrintSection("第 3 步：求解交点");

	// 直线 - 直线交点
	MLine l1(MPoint(0.0, 0.0, 0.0), MVector(1.0, 1.0, 0.0));
	MLine l2(MPoint(0.0, 2.0, 0.0), MVector(1.0, -1.0, 0.0));
	MPoint pLL;
	bool bLL = l1.Intersect(l2, pLL);
	PrintPoint("直线 l1 ∩ l2 的交点", pLL);
	Check(bLL && Dist(pLL, MPoint(1.0, 1.0, 0.0)) < 1e-7, "交点应为 (1,1,0)");

	// 线段 - 线段交点（与直线演示错开位置，便于可视化区分）
	MLineSeg s1(MPoint(3.0, 0.0, 0.0), MPoint(5.0, 2.0, 0.0));
	MLineSeg s2(MPoint(3.0, 2.0, 0.0), MPoint(5.0, 0.0, 0.0));
	MPoint pSS;
	bool bSS = s1.Intersect(s2, pSS);
	PrintPoint("线段 s1 ∩ s2 的交点", pSS);
	Check(bSS && Dist(pSS, MPoint(4.0, 1.0, 0.0)) < 1e-7, "交点应为 (4,1,0)");

	// 直线 - 平面交点（平面 z = 0，直线从 (0,0,5) 沿 -Z 方向）
	MVector nrm(0.0, 0.0, 1.0);
	MPoint origin(0.0, 0.0, 0.0);
	MPlane plane(nrm, origin);
	MLine lp(MPoint(0.0, 0.0, 5.0), MVector(0.0, 0.0, -1.0));
	MPoint pLP;
	bool bLP = plane.Intersect(lp, pLP);
	PrintPoint("直线与平面 z=0 的交点", pLP);
	Check(bLP && Dist(pLP, MPoint(0.0, 0.0, 0.0)) < 1e-7, "交点应为 (0,0,0)");

	// 线段 - 平面交点
	MLineSeg sp(MPoint(0.0, 0.0, 5.0), MPoint(0.0, 0.0, -5.0));
	MPoint pSP;
	bool bSP = plane.Intersect(sp, pSP);
	PrintPoint("线段与平面 z=0 的交点", pSP);
	Check(bSP && Dist(pSP, MPoint(0.0, 0.0, 0.0)) < 1e-7, "交点应为 (0,0,0)");

	// 平面 - 平面交线（z=0 ∩ y=0 -> X 轴直线）
	MPoint o2(0.0, 0.0, 0.0), px(1.0, 0.0, 0.0), pz(0.0, 0.0, 1.0);
	MPlane planeY(o2, px, pz);            // 由三点确定的平面：y = 0
	MLine linePP;
	bool bPP = plane.Intersect(planeY, linePP);
	PrintPoint("平面 z=0 ∩ y=0 的交线经过点", linePP.Point());
	PrintVector("交线方向", linePP.Direction());
	Check(bPP && fabs(plane.DistanceToPoint(linePP.Point(), true)) < 1e-7, "交线上的点应在平面 z=0 上");
	Check(bPP && fabs(planeY.DistanceToPoint(linePP.Point(), true)) < 1e-7, "交线上的点应在平面 y=0 上");

	// 供可视化使用
	m_pScene->lineIntersection = pLL;
	m_pScene->seg1Start.Set(3.0, 0.0, 0.0);
	m_pScene->seg1End.Set(5.0, 2.0, 0.0);
	m_pScene->seg2Start.Set(3.0, 2.0, 0.0);
	m_pScene->seg2End.Set(5.0, 0.0, 0.0);
	m_pScene->segIntersection = pSS;
	m_pScene->bShowSegs = true;
	m_pScene->plane = plane;
	m_pScene->linePlaneStart.Set(0.0, 0.0, 5.0);
	m_pScene->linePlaneEnd.Set(0.0, 0.0, -5.0);
	m_pScene->planeHit = pLP;
	m_pScene->bShowPlane = true;
}

//-----------------------------------------------------------------------------
// 4 判断空间范围
//-----------------------------------------------------------------------------
void GeomDemo::DemoRange()
{
	PrintSection("第 4 步：判断空间范围");

	MBBox box;
	box.Set(MPoint(0.0, 0.0, 0.0), MPoint(10.0, 10.0, 10.0));

	MPoint inP(5.0, 5.0, 5.0);
	MPoint outP(11.0, 5.0, 5.0);
	Check(box.IsPointWithinBBox(inP), "(5,5,5) 应在包围盒内");
	Check(!box.IsPointWithinBBox(outP), "(11,5,5) 应在包围盒外");

	cout << "    包围盒对角线长 = " << box.Diagonal() << endl;
	PrintPoint("包围盒中心", box.Center());

	// 8 个角点
	MPoint corners[8];
	box.Get8Corners(corners);
	cout << "    包围盒 8 个角点:" << endl;
	for (int i = 0; i < 8; i++)
	{
		cout << "      " << PtStr(corners[i]) << endl;
	}

	// 沿 X 方向在 x=5 处拆分
	MBBox sub1, sub2;
	if (box.Split(5.0, 0, sub1, sub2))
	{
		PrintPoint("拆分后子盒1 min", sub1.GetMinPnt());
		PrintPoint("拆分后子盒1 max", sub1.GetMaxPnt());
		PrintPoint("拆分后子盒2 min", sub2.GetMinPnt());
		PrintPoint("拆分后子盒2 max", sub2.GetMaxPnt());
	}

	// 包围盒求并
	MBBox b1, b2;
	b1.Set(MPoint(0.0, 0.0, 0.0), MPoint(2.0, 2.0, 2.0));
	b2.Set(MPoint(1.0, 1.0, 1.0), MPoint(5.0, 5.0, 5.0));
	MBBox bu;
	bu.Set(MPoint(0.0, 0.0, 0.0), MPoint(2.0, 2.0, 2.0));
	bu.Unite(b2);
	PrintPoint("b1 ∪ b2 之后 min", bu.GetMinPnt());
	PrintPoint("b1 ∪ b2 之后 max", bu.GetMaxPnt());

	// 包围盒是否相离
	MBBox d1, d2, d3;
	d1.Set(MPoint(0.0, 0.0, 0.0), MPoint(2.0, 2.0, 2.0));
	d2.Set(MPoint(5.0, 5.0, 5.0), MPoint(6.0, 6.0, 6.0));
	d3.Set(MPoint(1.0, 1.0, 1.0), MPoint(3.0, 3.0, 3.0));
	Check(d1.IsDisjoint(d2), "包围盒 d1 与 d2 应相离");
	Check(!d1.IsDisjoint(d3), "包围盒 d1 与 d3 应相交(不相离)");

	// 供可视化使用
	m_pScene->box = box;
	m_pScene->boxPtIn = inP;
	m_pScene->boxPtOut = outP;
	m_pScene->bShowBox = true;
}

//-----------------------------------------------------------------------------
// 5 进行矩阵变换
//-----------------------------------------------------------------------------
void GeomDemo::DemoTransform()
{
	PrintSection("第 5 步：进行矩阵变换");

	// 绕 Z 轴旋转 90 度的旋转矩阵
	MMatrix rotZ(3, 3);
	rotZ = MMatrix::s_GetRotationMatrix(MVector(0.0, 0.0, 1.0), 3.14159265358979 / 2.0);
	PrintMatrix("绕 Z 轴 90° 的旋转矩阵 rotZ", rotZ);

	// 用矩阵变换点
	MPoint pIn(1.0, 0.0, 0.0);
	MPoint pOut = rotZ * pIn;
	PrintPoint("旋转前点 pIn", pIn);
	PrintPoint("旋转后点 pOut = rotZ·pIn", pOut);

	// 用矩阵变换向量
	MVector vOut = rotZ * MVector(1.0, 0.0, 0.0);
	PrintVector("旋转后向量 = rotZ·(1,0,0)", vOut);

	// 由“源向量 -> 目标向量”构造旋转矩阵
	MMatrix rotMap(3, 3);
	rotMap = MMatrix::s_GetRotationMatrix(MVector(1.0, 0.0, 0.0), MVector(0.0, 1.0, 0.0));
	MPoint mapped = rotMap * MPoint(1.0, 0.0, 0.0);
	PrintPoint("把 X 轴旋到 Y 轴后，(1,0,0) 变为", mapped);

	// 转置
	MMatrix trans(3, 3);
	trans = rotZ;               // 深拷贝
	trans.Transpose();
	PrintMatrix("rotZ 的转置 rotZᵀ", trans);

	// 矩阵乘法：两个旋转复合（先绕 Y 再绕 X）
	MMatrix rx(3, 3), ry(3, 3);
	rx = MMatrix::s_GetRotationMatrix(MVector(1.0, 0.0, 0.0), 3.14159265358979 / 4.0);
	ry = MMatrix::s_GetRotationMatrix(MVector(0.0, 1.0, 0.0), 3.14159265358979 / 4.0);
	MMatrix comb(3, 3);
	comb = rx * ry;              // 复合矩阵
	PrintMatrix("复合矩阵 rx * ry", comb);
	MPoint combPnt = comb * MPoint(1.0, 0.0, 0.0);
	PrintPoint("复合旋转作用在 (1,0,0) 上", combPnt);

	// 用 LU 分解求解线性方程组 A·x = b
	// 该矩阵消元后最后一个对角线为 0，早期版本的 LU 会误判为奇异（已修复）
	MMatrix A(3, 3);
	A[0][0] = 2.0; A[0][1] = 1.0; A[0][2] = 1.0;
	A[1][0] = 1.0; A[1][1] = 3.0; A[1][2] = 2.0;
	A[2][0] = 1.0; A[2][1] = 0.0; A[2][2] = 0.0;
	// 取 x = (3, 1, 2)，则 b = A·x = (9, 10, 3)
	double rhs[3] = { 9.0, 10.0, 3.0 };

	MMatrix Acopy(3, 3);
	Acopy = A;                   // LU 分解会原地修改，先深拷贝一份
	vector<int> idx;
	bool bLU = Acopy.LUDecompose(idx);
	double sol[3] = { 0.0, 0.0, 0.0 };
	if (bLU)
	{
		bLU = Acopy.GetSolutionAfterLUDecompose(idx, rhs);   // 解会写回 rhs
	}
	if (bLU)
	{
		sol[0] = rhs[0];
		sol[1] = rhs[1];
		sol[2] = rhs[2];
	}
	cout << "    LU 求解 A·x = b 得到 x = ("
		<< sol[0] << ", " << sol[1] << ", " << sol[2] << ")" << endl;
	Check(bLU && fabs(sol[0] - 3.0) < 1e-7 && fabs(sol[1] - 1.0) < 1e-7 && fabs(sol[2] - 2.0) < 1e-7,
		"解应等于 (3,1,2)");
}

//-----------------------------------------------------------------------------
// 6 验证变换后的结果
//-----------------------------------------------------------------------------
void GeomDemo::DemoVerify()
{
	PrintSection("第 6 步：验证变换后的结果");

	MMatrix rotZ(3, 3);
	rotZ = MMatrix::s_GetRotationMatrix(MVector(0.0, 0.0, 1.0), 3.14159265358979 / 2.0);

	// 1) 点旋转结果验证：R·(1,0,0) 应为 (0,1,0)
	MPoint pA(1.0, 0.0, 0.0);
	MPoint pA_ = rotZ * pA;
	Check(Dist(pA_, MPoint(0.0, 1.0, 0.0)) < 1e-9, "rotZ·(1,0,0) = (0,1,0)");

	// 2) 正交性验证：Rᵀ·R 应等于单位阵
	MMatrix RT(3, 3);
	RT = rotZ;
	RT.Transpose();
	MMatrix prod(3, 3);
	prod = RT * rotZ;
	Check(prod.IsIdentity(), "Rᵀ·R 应等于单位阵（旋转矩阵正交性）");

	// 3) 距离保持验证：|R·a - R·b| == |a - b|
	MPoint qA(3.0, 0.0, 1.0), qB(1.0, 2.0, -1.0);
	MPoint rA = rotZ * qA;
	MPoint rB = rotZ * qB;
	Check(fabs(Dist(qA, qB) - Dist(rA, rB)) < 1e-9, "旋转前后两点距离保持不变");

	// 4) 源向量到目标向量映射验证
	MMatrix rotMap(3, 3);
	rotMap = MMatrix::s_GetRotationMatrix(MVector(1.0, 0.0, 0.0), MVector(0.0, 1.0, 0.0));
	MVector map = rotMap * MVector(1.0, 0.0, 0.0);
	Check(map == MVector(0.0, 1.0, 0.0), "由 X 轴映射到 Y 轴的旋转应把 (1,0,0) 变为 (0,1,0)");

	// 5) 三角形旋转变换：原始与变换后顶点到旋转轴(Z 轴)距离不变
	MPoint tri[3];
	tri[0].Set(1.0, 0.0, 0.0);
	tri[1].Set(2.0, 0.0, 0.0);
	tri[2].Set(1.0, 1.0, 0.0);
	MPoint tri2[3];
	for (int i = 0; i < 3; i++)
	{
		tri2[i] = rotZ * tri[i];
		m_pScene->triOrig[i] = tri[i];
		m_pScene->triTrans[i] = tri2[i];
	}
	m_pScene->bShowTri = true;
	for (int i = 0; i < 3; i++)
	{
		double d0 = tri[i][0] * tri[i][0] + tri[i][1] * tri[i][1];   // 到 Z 轴距离的平方
		double d1 = tri2[i][0] * tri2[i][0] + tri2[i][1] * tri2[i][1];
		Check(fabs(d0 - d1) < 1e-9, "三角形顶点到旋转轴距离旋转后保持不变");
	}

	// 6) 平面旋转验证：平面 z=0 绕 X 轴转 90° 后成为平面 y=0
	MMatrix rotX(3, 3);
	rotX = MMatrix::s_GetRotationMatrix(MVector(1.0, 0.0, 0.0), 3.14159265358979 / 2.0);
	MVector nrm(0.0, 0.0, 1.0);
	MPoint origin(0.0, 0.0, 0.0);
	MPlane planeZ(nrm, origin);                       // z = 0

	MPlane planeY2(planeZ);                           // 复制平面
	planeY2.Transform(rotX);                          // 用库内的平面变换
	PrintVector("旋转后平面法向", planeY2.Normal());
	PrintPoint("旋转后平面上一点", planeY2.GetArbitraryPoint());

	Check(planeY2.Normal() == MVector(0.0, -1.0, 0.0) || planeY2.Normal() == MVector(0.0, 1.0, 0.0),
		"平面法向应转到 Y 轴方向");
	Check(fabs(planeY2.DistanceToPoint(MPoint(0.0, 0.0, 0.0), true)) < 1e-7,
		"旋转后的平面仍过原点");

	// 取 z=0 上一点 (3,4,0)，旋转后应在变换后的平面上
	MPoint onPlane = rotX * MPoint(3.0, 4.0, 0.0);
	PrintPoint("z=0 上点 (3,4,0) 旋转后", onPlane);
	Check(fabs(planeY2.DistanceToPoint(onPlane, true)) < 1e-7,
		"旋转后的点应仍在旋转后的平面上");

	// 7) Jacobi 特征值验证：对称矩阵 [[2,1,0],[1,2,1],[0,1,2]]
	MMatrix EigA(3, 3);
	EigA[0][0] = 2.0; EigA[0][1] = 1.0; EigA[0][2] = 0.0;
	EigA[1][0] = 1.0; EigA[1][1] = 2.0; EigA[1][2] = 1.0;
	EigA[2][0] = 0.0; EigA[2][1] = 1.0; EigA[2][2] = 2.0;
	MMatrix EigV(3, 3);
	double ev[3] = { 0.0, 0.0, 0.0 };
	if (EigA.JacobiEigen(ev, EigV))
	{
		cout << "    特征值 = " << ev[0] << ", " << ev[1] << ", " << ev[2] << endl;
		// 对每个特征对验证 A·v = λ·v
		bool bEigOk = true;
		for (int c = 0; c < 3; c++)
		{
			MPoint v(EigV[0][c], EigV[1][c], EigV[2][c]);
			MPoint Av = EigA * v;
			for (int r = 0; r < 3; r++)
			{
				if (fabs(Av[r] - ev[c] * v[r]) > 1e-6)
				{
					bEigOk = false;
				}
			}
		}
		Check(bEigOk, "Jacobi 求得的每个特征对都满足 A·v = λ·v");
	}
	else
	{
		Check(false, "JacobiEigen 调用失败");
	}
}
