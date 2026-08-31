#include "pch.h"
#include"MPoint.h"

class PointCompare
{
public:
	PointCompare(double dEps = 1.0e-7) : m_tol(dEps) {}
	bool operator()(const MPoint& p0, const MPoint& p1) const;

private:
	double m_tol;	// tolerance
};