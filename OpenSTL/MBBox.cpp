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

void MBBox::Set(const MPoint& minPoint, const MPoint& maxPoint)
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
	const MVector d = cursorRay.Direction();
	const MPoint p = cursorRay.Point();
	double maxT = 1.0e10;
	double minT = -maxT;
	for (int i = 0; i < 3; i++) {
		if (p[i]<this->m_minPoint[i] || p[i] > this->m_maxPoint[i]) {
			return false;
		}
		else {
			double t = 1.0 / d[i];
			double minV = (m_minPoint[i] - p[i]) * t;
			double maxV = (m_maxPoint[i] - p[i]) * t;
			if (maxV < minV) {
				swap(maxV, minV);
			}
			maxT = min(maxT, maxV);
			minT = max(minT, minV);
			if (maxT < minT) {
				return true;
			}
		}
	}
	return true;
}

bool MBBox::IsDisjoint(const MBBox& box) const
{
	const MPoint& minPnt = box.GetMinPnt();
	const MPoint& maxPnt = box.GetMaxPnt();

	return   m_minPoint[0] > maxPnt[0] || m_minPoint[1] > maxPnt[1] || m_minPoint[2] > maxPnt[2]
		|| m_maxPoint[0] < minPnt[0] || m_maxPoint[1] < minPnt[1] || m_maxPoint[2] < minPnt[2];
}

void MBBox::Write(ostream& os, bool binary) const
{
	m_minPoint.Write(os, binary);
	m_maxPoint.Write(os, binary);
}

bool MBBox::Read(ifstream& is, bool binary)
{
	return m_minPoint.Read(is, binary) && m_maxPoint.Read(is, binary);
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

void MBBox::Clear()
{
	m_minPoint.Set(0.0, 0.0, 0.0);
	m_maxPoint.Set(0.0, 0.0, 0.0);
}

double MBBox::Diagonal() const
{
	return m_minPoint.DistanceToPoint(m_maxPoint);
}

MPoint MBBox::Center() const
{
	return m_minPoint + (m_maxPoint - m_minPoint) / 2;
}

double MBBox::DistanceToPoint(const MPoint& point) const
{
	double dist = 0.0;
	for (int i = 0; i < 3; i++)
	{
		if (point[i] < m_minPoint[i])
		{
			double t = m_minPoint[i] - point[i];
			dist += t * t;
		}
		else if (point[i] > m_maxPoint[i])
		{
			double t = point[i] - m_maxPoint[i];
			dist += t * t;
		}
	}

	return dist;
}

bool MBBox::Split(double splitValue, int dir, MBBox& subBox1, MBBox& subBox2) const
{
	if (splitValue <= m_minPoint[dir] || splitValue >= m_maxPoint[dir])
		return false;
	MPoint minPoint= m_minPoint;
	MPoint maxPoint = m_maxPoint;
	minPoint[dir] = splitValue;
	maxPoint[dir] = splitValue;
	subBox1.Set(m_minPoint, minPoint);
	subBox2.Set(maxPoint, m_maxPoint);
	return true;
}

void MBBox::Get8Corners(MPoint cornerPnts[8]) const
{
	// bottom
	cornerPnts[0].Set(m_minPoint[0], m_minPoint[1], m_minPoint[2]);
	cornerPnts[1].Set(m_minPoint[0], m_minPoint[1], m_maxPoint[2]);
	cornerPnts[2].Set(m_maxPoint[0], m_minPoint[1], m_maxPoint[2]);
	cornerPnts[3].Set(m_maxPoint[0], m_minPoint[1], m_minPoint[2]);
	// top
	cornerPnts[4].Set(m_minPoint[0], m_maxPoint[1], m_minPoint[2]);
	cornerPnts[5].Set(m_minPoint[0], m_maxPoint[1], m_maxPoint[2]);
	cornerPnts[6].Set(m_maxPoint[0], m_maxPoint[1], m_minPoint[2]);
	cornerPnts[7].Set(m_maxPoint[0], m_maxPoint[1], m_maxPoint[2]);
}

bool MBBox::IsPointWithinBBox(const MPoint& pt) const
{
	for (int i = 0; i < 3; i++) {
		if (pt[i]<m_minPoint[i] || pt[i] > m_maxPoint[i]) {
			return false;
		}
	}
	return true;
}
