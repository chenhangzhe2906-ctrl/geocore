#include "pch.h"
#include "MLine.h"
MLine* MLine::LeastSquareFit(const vector<MPoint>& points)
{
	int size = (int)points.size();
	if (size < 2)
	{
		return NULL;
	}

	MPoint centre(0, 0, 0);
	for (int i = 0; i < size; i++)
	{
		centre += points[i];
	}
	centre /= size;

	double maxDist = 0.0;
	int idx = -1;
	for (int i = 0; i < size; i++)
	{
		double dist = points[i].DistanceToPoint(centre);
		if (dist > maxDist)
		{
			maxDist = dist;
			idx = i;
		}
	}
	ASSERT(idx != -1);

	MVector refDir = points[idx] - centre;
	MVector lineDir(0.0, 0.0, 0.0);
	for (int i = 0; i < size; i++)
	{
		MVector dif = points[i] - centre;
		if (dif % refDir < 0.0)
		{
			dif.Flip();
		}

		lineDir += dif;
	}

	lineDir /= size;
	lineDir.Normalize();
	return new MLine(centre, lineDir);
}

MLine::MLine(void)
{
}

MLine::MLine(const MPoint& point, const MVector& dir)
{
	m_dir = dir;
	m_dir.Normalize();
	m_point = point;
	return;
}

MLine::MLine(const MPoint& p1, const MPoint& p2)
{
	m_point = p1;
	m_dir = p2 - p1;
	m_dir.Normalize();
	return;
}

MLine::MLine(const MVector& v1, const MVector& v2)
{
	m_point.Set(v1[0], v1[1], v1[2]);
	m_dir = v2 - v1;
	m_dir.Normalize();
}

MLine::~MLine(void)
{
}

void MLine::Set(const MPoint& point, const MVector& dir)
{
	m_dir = dir;
	m_dir.Normalize();
	m_point = point;
}

double MLine::DistanceToPoint(const MPoint& point) const
{
	MPoint p = ProjectPoint(point);
	return p.DistanceToPoint(point);
}

MPoint MLine::ProjectPoint(const MPoint& point, double* t) const
{
	MVector vec = point - m_point;
	double param = vec % m_dir;
	if (t != NULL)
	{
		*t = param;
	}
	return MPoint(m_point + param * m_dir);
}

MVector MLine::ProjectVector(const MVector& v, double* t) const
{
	double param = v % m_dir;
	if (t != NULL) {
		*t = param;
	}
	return MVector(param * m_dir);
}

bool MLine::Intersect(const MLine& line, MPoint& intPnt) const
{
	const double epsilon = 1e-8;
	MPoint a_pnt = this->m_point;
	MPoint b_pnt = line.m_point;
	MVector c_vec(b_pnt - a_pnt);
	if (fabs((this->m_dir * line.m_dir) % c_vec) <= epsilon) {
		if (this->m_dir != line.m_dir) {
			double t = ((c_vec * line.m_dir) % (this->m_dir * line.m_dir)) / ((this->m_dir * line.m_dir) % (this->m_dir * line.m_dir));
			intPnt = this->m_point + t * this->m_dir;
			return true;
		}
	}
	return false;
}

MLineSeg::MLineSeg()
{
}

MLineSeg::MLineSeg(const MPoint& startPnt, const MPoint& endPnt)
{
	m_endPnt = endPnt;
	m_startPnt = startPnt;
}

MVector MLineSeg::Direction() const
{
	MVector a=MVector(m_endPnt - m_startPnt);
	a.Normalize();
	return MVector(a);
}


bool MLineSeg::Intersect(const MLineSeg& lsg, MPoint& intPnt) const
{
	const MPoint& P1 = m_startPnt;
	MVector V1 = m_endPnt - m_startPnt;
	double m1 = V1.Magnitude();
	if (fabs(m1) < ERR7)//L1退化为一点
	{
		if (lsg.IsContain(P1))
		{
			intPnt = P1;
			return true;
		}
		return false;
	}
	V1.Normalize();

	const MPoint& P2 = lsg.GetStartPoint();
	MVector V2 = lsg.GetEndPoint() - P2;
	double m2 = V2.Magnitude();
	if (fabs(m2) < ERR7)//L2退化为一点
	{
		if (IsContain(P2))
		{
			intPnt = P2;
			return true;
		}
		return false;
	}
	V2.Normalize();

	MVector P21 = P2 - P1;
	double a[3];
	a[0] = 1.0; //V1 % V1;
	a[1] = V1 % V2;
	a[2] = 1.0; //V2 % V2;
	double den = a[1] * a[1] - 1.0;


	if (fabs(den) < ERR8)//共线
	{
		if (IsContain(lsg.GetStartPoint()))
		{
			intPnt = lsg.GetStartPoint();
			return true;
		}
		else if (IsContain(lsg.GetEndPoint()))
		{
			intPnt = lsg.GetEndPoint();
			return true;
		}
		else if (lsg.IsContain(m_startPnt))
		{
			intPnt = m_startPnt;
			return true;
		}
		else if (lsg.IsContain(m_endPnt))
		{
			intPnt = m_endPnt;
			return true;
		}
		return false;
	}
	double b[2];
	b[0] = V1 % P21;
	b[1] = V2 % P21;

	double numa = a[1] * b[1] - b[0];
	double numb = b[1] - a[1] * b[0];

	double alpha = numa / den;
	double beta = numb / den;

	double t = alpha / m1;
	if (t < -ERR8 || t > 1.0 + ERR8)
		return false;

	double s = beta / m2;
	if (s < -ERR8 || s > 1.0 + ERR8)
		return false;

	intPnt = P1 + V1 * alpha;
	return true;
}

int MLineSeg::IntersectNew(const MLineSeg& lsg, MPoint& intPnt) const
{
	const MPoint& P1 = m_startPnt;
	MVector V1 = m_endPnt - m_startPnt;
	double m1 = V1.Magnitude();
	if (fabs(m1) < ERR7)//L1退化为一点
	{
		if (lsg.IsContain(P1))
		{
			intPnt = P1;
			return 1;
		}
		return 0;
	}
	V1.Normalize();

	const MPoint& P2 = lsg.GetStartPoint();
	MVector V2 = lsg.GetEndPoint() - P2;
	double m2 = V2.Magnitude();
	if (fabs(m2) < ERR7)//L2退化为一点
	{
		if (IsContain(P2))
		{
			intPnt = P2;
			return 1;
		}
		return 0;
	}
	V2.Normalize();

	MVector P21 = P2 - P1;
	double a[3];
	a[0] = 1.0; //V1 % V1;
	a[1] = V1 % V2;
	a[2] = 1.0; //V2 % V2;
	double den = a[1] * a[1] - 1.0;


	if (fabs(den) < ERR8)//共线
	{
		if (IsContain(lsg.GetStartPoint()))
		{
			intPnt = lsg.GetStartPoint();
			return -1;
		}
		else if (IsContain(lsg.GetEndPoint()))
		{
			intPnt = lsg.GetEndPoint();
			return -1;
		}
		else if (lsg.IsContain(m_startPnt))
		{
			intPnt = m_startPnt;
			return -1;
		}
		else if (lsg.IsContain(m_endPnt))
		{
			intPnt = m_endPnt;
			return -1;
		}
		return 0;
	}

	double b[2];
	b[0] = V1 % P21;
	b[1] = V2 % P21;

	double numa = a[1] * b[1] - b[0];
	double numb = b[1] - a[1] * b[0];

	double alpha = numa / den;
	double beta = numb / den;

	double t = alpha / m1;
	if (t < -ERR8 || t > 1.0 + ERR8)
		return 0;

	double s = beta / m2;
	if (s < -ERR8 || s > 1.0 + ERR8)
		return 0;

	intPnt = P1 + V1 * alpha;
	return 1;
}

bool MLineSeg::IsCoincidence(const MLineSeg& seg) const
{
	const MPoint& sPt = seg.GetStartPoint();
	const MPoint& ePt = seg.GetEndPoint();

	if (sPt == m_startPnt && ePt == m_endPnt)
	{
		return true;
	}
	else if (sPt == m_endPnt && ePt == m_startPnt)
	{
		return true;
	}

	return false;
}

bool MLineSeg::IsContain(const MPoint& pt) const
{
	if (pt == m_startPnt || pt == m_endPnt)
	{
		return true;
	}

	MVector vecSeg = m_endPnt - m_startPnt;
	double lenSeg = vecSeg.Magnitude();
	if (lenSeg < ERR7)//线段退化为一点
	{
		return false;
	}

	MVector vecSp = pt - m_startPnt;
	double lenSp = vecSp.Magnitude();

	vecSeg.Normalize();
	vecSp.Normalize();

	double dot = vecSeg % vecSp;

	double dis = DistanceToPoint(pt);	//确保精度范围内
	const double epsilon = 1e-7;
	if (fabs(dot - 1.0)<epsilon && lenSp < lenSeg &&fabs(dis)<epsilon)
	{
		return true;
	}
	return false;
}

double MLineSeg::GetLength() const
{
	return m_startPnt.DistanceToPoint(m_endPnt);
}

double MLineSeg::DistanceToPoint(const MPoint& point) const
{
	MVector dir = this->Direction();
	dir.Normalize();

	return 0.0;
}




