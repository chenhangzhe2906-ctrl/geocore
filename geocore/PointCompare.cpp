#include "pch.h"
#include "PointCompare.h"

bool PointCompare::operator()(const MPoint& p0, const MPoint& p1) const
{
	if (fabs(p0[0] - p1[0]) <= m_tol)
	{
		if (fabs(p0[1] - p1[1]) <= m_tol)
			return (p0[2] < p1[2] - m_tol);
		else
			return (p0[1] < p1[1] - m_tol);
	}
	else return (p0[0] < p1[0] - m_tol);
}