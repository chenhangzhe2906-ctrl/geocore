#pragma once
//#include "TcGlobalDef.h"
#include "MGeometry.h"
#include "MVector.h"
#include "MPoint.h"

class MLine : public MGeometry
{
public:
	static MLine* LeastSquareFit(const vector<MPoint>& points);

public:
	MLine(void);
	MLine(const MPoint& point, const MVector& dir);
	MLine(const MPoint& p1, const MPoint& p2);
	MLine(const MVector& v1, const MVector& v2);
	virtual ~MLine(void);

public:
	void Set(const MPoint& point, const MVector& dir);
	const MPoint& Point() const { return m_point; }
	const MVector& Direction() const { return m_dir; }

public:
	double DistanceToPoint(const MPoint& point) const;
	MPoint ProjectPoint(const MPoint& point, double* t = NULL) const;
	MVector ProjectVector(const MVector& v, double* t = NULL) const;
	bool Intersect(const MLine& line, MPoint& intPnt) const;

public:
	void Write(ostream& os, bool binary = true) const;
	bool Read(istream& is, bool binary = true);

private:
	MPoint m_point;
	MVector m_dir;
};

class MLineSeg : public MGeometry
{
public:
	MLineSeg();
	MLineSeg(const MPoint& startPnt, const MPoint& endPnt);

public:
	const MPoint& GetStartPoint() const { return m_startPnt; }
	const MPoint& GetEndPoint() const { return m_endPnt; }
	MVector Direction() const;
	//bool IsIntersectWithLineSeg(const MLineSeg& lineSeg);
	bool Intersect(const MLineSeg& lsg, MPoint& intPnt) const;

	// 相交返回1，并输出交点；共线返回-1,输出交点为包含的端点；不相交返回0
	int IntersectNew(const MLineSeg& lsg, MPoint& intPnt) const;

	bool IsCoincidence(const MLineSeg& seg)const;//线段是否重合
	bool IsContain(const MPoint& pt)const;//点在线段上

	void SetStartPoint(const MPoint& pt) { m_startPnt = pt; }
	void SetEndPoint(const MPoint& pt) { m_endPnt = pt; }

	double GetLength() const;

	// 指定点与线段最近点之间的距离
	double DistanceToPoint(const MPoint& point) const;

	// 将给定点投影到线段上
	// 如果投影点在线段上，直接返回
	// 如果投影点不在线段上，则返回最近的端点
	MPoint ProjectPoint(const MPoint& point, double* t = NULL) const;

private:
	MPoint m_startPnt;
	MPoint m_endPnt;
};
