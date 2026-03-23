#include "pch.h"
#include "MBBox.h"

MBBox::MBBox()
{
	m_minPoint.Set(0.0, 0.0, 0.0);
	m_maxPoint.Set(0.0, 0.0, 0.0);
}

MBBox::MBBox(MPoint& minPoint, MPoint& maxPoint)
{
	Set(minPoint, maxPoint);
}

MBBox::~MBBox()
{
}

void MBBox::Set(MPoint& minPoint, MPoint& maxPoint)
{
	m_maxPoint = maxPoint;
	m_minPoint = minPoint;
	return;
}

void MBBox::CalculateFromPoints(const vector<MPoint>& points)
{
	const double inf = 1e10;
	vector<double> cMin = { inf,inf,inf };
	vector <double> cMax = { -inf,-inf,-inf };
	for (auto& it : points) {
		for (int i = 0; i < 3; i++) {
			cMin[i] = min(cMin[i], it[i]);
			cMax[i] = max(cMax[i], it[i]);
		}
	}
	m_maxPoint.Set(cMax[0], cMax[1], cMax[2]);
	m_minPoint.Set(cMax[0], cMax[1], cMax[2]);
	return;
}

int MBBox::GetLongestDirection() const
{
	MVector longestVec(m_maxPoint - m_minPoint);
	for (int i = 0; i < 2; i++) {
		if (longestVec[i] == min(longestVec[0], longestVec[1], longestVec[2])) {
			return i;
		}
	}
}

void MBBox::Unite(const MBBox& other)
{
	if (IsValid() && other.IsValid())
	{
		const MPoint& minPnt = other.GetMinPnt();
		const MPoint& maxPnt = other.GetMaxPnt();

		for (int i = 0; i < 3; i++)
		{
			m_minPoint[i] = min(m_minPoint[i], minPnt[i]);
			m_maxPoint[i] = max(m_maxPoint[i], maxPnt[i]);
		}
	}
	else if (other.IsValid())
	{
		*this = other;
	}
}

bool MBBox::IsDisjoint(const MLine& cursorRay) const
{
	return false;
}

bool MBBox::IsDisjoint(const MBBox& box) const
{
	return false;
}

bool MBBox::IsValid() const
{
	const double epsilon = 1e-10;
	double dist = m_minPoint.DistanceToPoint(m_maxPoint);
	if (fabs(dist) <= epsilon) {
		return false;
	}
	else {
		return true;
	}
}


