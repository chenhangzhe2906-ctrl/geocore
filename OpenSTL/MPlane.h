#pragma once
#include "pch.h"
#include"MMatrix.h"
#include"MVector.h"
#include "MPoint.h"
#include "MLine.h"

class MPlane
{
public:
	MPlane(void);
	MPlane(const MVector& normal, const MPoint& pnt);
	MPlane(const MPoint& pnt1, const MPoint& pnt2, const MPoint& pnt3);
	~MPlane();

public:
	bool Set(const MVector& normal, const MPoint& pnt);

public:
	MVector Normal() const;
	double GetCoefD() const { return m_coef[3]; }
	void Offset(double dist);

public:
	//直线与平面交点，平行返回false
	bool Intersect(const MLine& Line, MPoint& point)const;
	//线段与平面求交 若该线段在平面上，返回seg中的一个点
	bool Intersect(const MLineSeg& seg, MPoint& point)const;
	//平面相交并计算直线，平行返回false
	bool Intersect(const MPlane& plane, MLine& line) const;
	MVector ProjectVector(const MVector& vector) const;
	MPoint ProjectPoint(const MPoint& point) const;
	double DistanceToPoint(const MPoint& point, bool bSign) const;
	void Flip();
	MPoint GetArbitraryPoint() const;
	void Transform(const MMatrix& tcMat);

public:
	void Write(ostream& os, bool binary = true) const;
	bool Read(ifstream& is, bool binary = true);

private:
	double m_coef[4];// ax + by + cz + d = 0


};

