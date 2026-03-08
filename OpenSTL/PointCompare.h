#include "pch.h"

class PointCompare
{
public:
	PointCompare(double dEps = 1.0e-7) : m_tol(dEps) {}

	bool operator()(const MPoint& p0, const MPoint& p1) const
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

private:
	double m_tol;	// tolerance
};