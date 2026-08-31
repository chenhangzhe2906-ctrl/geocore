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
	m_point = point;
	Set(point, dir);
}

MLine::MLine(const MPoint& p1, const MPoint& p2)
{
	m_point = p1;
	m_dir = p2 - p1;
	if (m_dir.Magnitude() > ERR7)
	{
		m_dir.Normalize();
	}
	else
	{
		m_dir.SetZero();
	}
}

MLine::MLine(const MVector& v1, const MVector& v2)
{
	m_point.Set(v1[0], v1[1], v1[2]);
	m_dir = v2 - v1;
	if (m_dir.Magnitude() > ERR7)
	{
		m_dir.Normalize();
	}
	else
	{
		m_dir.SetZero();
	}
}

MLine::~MLine(void)
{
}

void MLine::Write(ostream& os, bool binary) const
{
	(void)binary;
	m_point.Write(os, false);
	double value[3];
	m_dir.GetValue(value);
	os << value[0] << " " << value[1] << " " << value[2] << endl;
}

bool MLine::Read(istream& is, bool binary)
{
	(void)binary;
	double point[3] = { 0.0, 0.0, 0.0 };
	double dir[3] = { 0.0, 0.0, 0.0 };
	if (!(is >> point[0] >> point[1] >> point[2] >> dir[0] >> dir[1] >> dir[2]))
	{
		return false;
	}
	MPoint p(point[0], point[1], point[2]);
	MVector v(dir[0], dir[1], dir[2]);
	Set(p, v);
	return !v.IsZero();
}
void MLine::Set(const MPoint& point, const MVector& dir)
{
	m_point = point;
	m_dir = dir;
	if (m_dir.Magnitude() > ERR7)
	{
		m_dir.Normalize();
	}
	else
	{
		m_dir.SetZero();
	}
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
	if (m_dir.Magnitude() <= epsilon || line.m_dir.Magnitude() <= epsilon)
	{
		return false;
	}
	MVector c_vec(line.m_point - m_point);
	MVector cross = m_dir * line.m_dir;
	double crossLen2 = cross.Length2();
	if (crossLen2 <= epsilon * epsilon)
	{
		return false;       // 平行或重合：没有唯一交点
	}
	if (fabs(cross % c_vec) > epsilon)
	{
		return false;       // 异面直线
	}
	double t = ((c_vec * line.m_dir) % cross) / crossLen2;
	intPnt = m_point + t * m_dir;
	return true;
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
	MVector AB = m_endPnt - m_startPnt;
	double len2 = AB.Length2();
	if (len2 <= ERR7 * ERR7)
	{
		return point.DistanceToPoint(m_startPnt);
	}
	MVector AP = point - m_startPnt;
	double ratio = (AP % AB) / len2;
	if (ratio <= 0.0)
	{
		return point.DistanceToPoint(m_startPnt);
	}
	if (ratio >= 1.0)
	{
		return point.DistanceToPoint(m_endPnt);
	}
	MPoint projection = m_startPnt + AB * ratio;
	return point.DistanceToPoint(projection);
}

MPoint MLineSeg::ProjectPoint(const MPoint& point, double* t) const
{
	MVector AP = point - m_startPnt;
	MVector AB = m_endPnt - m_startPnt;
	double len2_ab = AB.Length2();
	if (len2_ab <= ERR7 * ERR7)
	{
		if (t)
		{
			*t = 0.0;
		}
		return m_startPnt;
	}
	double ratio = (AP % AB) / len2_ab;
	if (t)
	{
		*t = ratio;
	}
	if (ratio <= 0.0)
	{
		return m_startPnt;
	}
	else if (ratio >= 1.0)
	{
		return m_endPnt;
	}
	else
	{
		return m_startPnt + AB * ratio;
	}
}




