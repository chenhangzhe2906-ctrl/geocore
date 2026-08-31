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
	// 修正：原实现把 min 也赋成了 cMax，导致包围盒退化成一个点；改为正确的 cMin
	if (points.empty())
	{
		Clear();
		return;
	}
	MPoint cMin = points[0];
	MPoint cMax = points[0];
	for (size_t k = 1; k < points.size(); k++)
	{
		for (int i = 0; i < 3; i++)
		{
			cMin[i] = min(cMin[i], points[k][i]);
			cMax[i] = max(cMax[i], points[k][i]);
		}
	}
	m_minPoint = cMin;
	m_maxPoint = cMax;
	return;
}

int MBBox::GetLongestDirection() const
{
	// 返回跨度最大的轴（0=X, 1=Y, 2=Z）
	MVector longestVec(m_maxPoint - m_minPoint);
	int idx = 0;
	if (longestVec[1] > longestVec[idx]) idx = 1;
	if (longestVec[2] > longestVec[idx]) idx = 2;
	return idx;
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
	// 使用标准 slab 算法判断无限直线是否与包围盒相交。
	// 原实现要求直线起点必须在盒内，并且对 d[i] == 0 做了除零，
	// 会把大量实际相交/不相交的情况判反。
	const MVector d = cursorRay.Direction();
	const MPoint p = cursorRay.Point();
	const double epsilon = 1e-12;
	double tMin = -1.0e100;
	double tMax = 1.0e100;
	for (int i = 0; i < 3; i++)
	{
		if (fabs(d[i]) <= epsilon)
		{
			// 平行于该轴向平面：若坐标不在区间内，整条直线都不相交。
			if (p[i] < m_minPoint[i] || p[i] > m_maxPoint[i])
			{
				return true;
			}
			continue;
		}

		double t1 = (m_minPoint[i] - p[i]) / d[i];
		double t2 = (m_maxPoint[i] - p[i]) / d[i];
		if (t1 > t2)
		{
			swap(t1, t2);
		}
		tMin = max(tMin, t1);
		tMax = min(tMax, t2);
		if (tMin > tMax)
		{
			return true;
		}
	}
	return false;
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
	// 修正：原实现返回平方距离，改为返回真实距离（与其它 DistanceToPoint 一致）
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

	return sqrt(dist);
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
