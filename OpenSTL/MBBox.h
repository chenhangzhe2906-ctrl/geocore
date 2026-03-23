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
	bool IsValid() const;
	void Set(MPoint& minPoint, MPoint& maxPoint);
	void CalculateFromPoints(const vector<MPoint>& points);
	int GetLongestDirection() const;
	void Unite(const MBBox& other);
	bool IsDisjoint(const MLine& cursorRay) const;
	bool IsDisjoint(const MBBox& box) const;
};

