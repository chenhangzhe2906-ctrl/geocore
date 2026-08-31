#pragma once
#include "pch.h"
#include "MMatrix.h"
#include "MPoint.h"
#include "MLine.h"

class MBBox
{
private:
	MPoint m_minPoint;
	MPoint m_maxPoint;

public:
	MBBox();
	MBBox(MPoint& minPoint, MPoint& maxPoint);
	virtual ~MBBox();

public:
	const MPoint& GetMinPnt() const { return m_minPoint; }
	const MPoint& GetMaxPnt() const { return m_maxPoint; }

public:
	void Clear();
	double Diagonal() const;
	MPoint Center() const;
	double DistanceToPoint(const MPoint& point) const;
	bool Split(double splitValue, int dir, MBBox& subBox1, MBBox& subBox2) const;
	void Get8Corners(MPoint cornerPnts[8]) const;
	bool IsPointWithinBBox(const MPoint& pt) const;

	bool IsValid() const;
	void Set(const MPoint& minPoint, const MPoint& maxPoint);
	void CalculateFromPoints(const vector<MPoint>& points);
	int GetLongestDirection() const;
	void Unite(const MBBox& other);
	bool IsDisjoint(const MLine& cursorRay) const;
	bool IsDisjoint(const MBBox& box) const;

public:
	void Write(ostream& os, bool binary = true) const;
	bool Read(ifstream& is, bool binary = true);

};

