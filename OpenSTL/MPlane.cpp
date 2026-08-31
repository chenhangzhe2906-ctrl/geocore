#include "pch.h"
#include "MPlane.h"

MPlane::MPlane(void)
{
	m_coef[0] = 0.0;
	m_coef[1] = 0.0;
	m_coef[2] = 0.0;
	m_coef[3] = 0.0;
}

MPlane::MPlane(const MVector& normal, const MPoint& pnt)
{
	m_coef[0] = 0.0;
	m_coef[1] = 0.0;
	m_coef[2] = 0.0;
	m_coef[3] = 0.0;
	Set(normal, pnt);
}

MPlane::MPlane(const MPoint& pnt1, const MPoint& pnt2, const MPoint& pnt3)
{
	m_coef[0] = 0.0;
	m_coef[1] = 0.0;
	m_coef[2] = 0.0;
	m_coef[3] = 0.0;
	MVector v1(pnt2 - pnt1);
	MVector v2(pnt3 - pnt1);
	MVector nor = v1 * v2;
	Set(nor, pnt1);
}

MPlane::~MPlane()
{
}

bool MPlane::Set(const MVector& normal, const MPoint& pnt)
{
	MVector unitNormal(normal);
	double length = unitNormal.Magnitude();
	if (length <= ERR7)
	{
		m_coef[0] = 0.0;
		m_coef[1] = 0.0;
		m_coef[2] = 0.0;
		m_coef[3] = 0.0;
		return false;
	}
	unitNormal /= length;
	m_coef[3] = 0.0;
	for (int i = 0; i < 3; i++) {
		m_coef[i] = unitNormal[i];
		m_coef[3] -= unitNormal[i] * pnt[i];
	}
	return true;
}

MVector MPlane::Normal() const
{
	return MVector(m_coef[0], m_coef[1], m_coef[2]);
}

void MPlane::Offset(double dist)//平面沿着法线方向整体平移
{
	m_coef[3] += dist;
	return;
}

bool MPlane::Intersect(const MLine& Line, MPoint& point) const
{
	MVector norm = this->Normal();
	MVector dir = Line.Direction();
	if (norm.Magnitude() <= ERR7 || dir.Magnitude() <= ERR7)
	{
		return false;
	}
	norm.Normalize();
	dir.Normalize();
	double denominator = norm % dir;
	if (fabs(denominator) < 1e-10) {
		return false;
	}
	double dist = DistanceToPoint(Line.Point(), true);
	double t = -dist / denominator;
	point = Line.Point() + t * dir;
	return true;
}

bool MPlane::Intersect(const MLineSeg& seg, MPoint& point) const
{
	MVector norm = this->Normal();
	MVector dir = seg.Direction();
	if (norm.Magnitude() <= ERR7)
	{
		return false;
	}
	norm.Normalize();
	if (dir.Magnitude() <= ERR7)
	{
		return fabs(DistanceToPoint(seg.GetStartPoint(), true)) <= ERR8;
	}
	dir.Normalize();
	double denominator = norm % dir;
	if (fabs(denominator) < 1e-10) {
		double a = DistanceToPoint(seg.GetStartPoint(), true);
		if (fabs(a) < 1e-10) {
			point = seg.GetStartPoint();
			return true;
		}
		return false;
	}
	double dist = DistanceToPoint(seg.GetStartPoint(), true);
	double t = -dist / denominator;
	MPoint pt = seg.GetStartPoint() + t * dir;
	if (!(seg.IsContain(pt)))
	{
		return false;
	}
	point = pt;
	return true;
}

bool MPlane::Intersect(const MPlane& plane, MLine& line) const
{
	MVector nor1 = this->Normal();
	MVector nor2 = plane.Normal();
	if (fabs((nor1*nor2).Magnitude()) < 1e-10) {
		return false;
	}
	MVector dir = nor1 * nor2;
	dir.Normalize();
	const double d1 = this->m_coef[3];
	const double d2 = plane.m_coef[3];
	// t = d2*n1 - d1*n2
	MVector t = nor1 * d2 - nor2 * d1;  
	// p0 = (t × (n1×n2)) / |n1×n2|^2
	// 注意：这里用未归一化的 (n1×n2) 更合适，所以重新算一次 u
	MVector u = nor1 * nor2;
	const double uu = u % u;
	MVector p0v = (t * u) / uu;         // (t×u)/uu，要求支持 向量/标量
	MPoint passPnt(p0v[0], p0v[1], p0v[2]);
	line.Set(passPnt, dir);
	return true;
}

MVector MPlane::ProjectVector(const MVector& vector) const
{
	MVector norm = Normal();
	return (vector - (vector % norm) * norm);
}

MPoint MPlane::ProjectPoint(const MPoint& point) const
{
	double dist = DistanceToPoint(point, true);
	return MPoint(point[0] - dist * m_coef[0],
		point[1] - dist * m_coef[1],
		point[2] - dist * m_coef[2]);
}

double MPlane::DistanceToPoint(const MPoint& point, bool Sign) const
{
	double dist = m_coef[0] * point[0] + m_coef[1] * point[1] + m_coef[2] * point[2] + m_coef[3];
	return Sign ? dist : fabs(dist);
}

void MPlane::Flip()
{
	for (auto& it : m_coef) {
		it *= -1;
	}
	return;
}

MPoint MPlane::GetArbitraryPoint() const
{
	int index = -1;
	for (int i = 0; i < 3; i++) {
		if (fabs(m_coef[i]) > ERR8) {
			index = i;
			break;
		}
	}
	if (index < 0)
	{
		return MPoint();   // 零法向量不是有效平面
	}
	MPoint arbitraryPoint(0.0, 0.0, 0.0);
	arbitraryPoint[index] = -m_coef[3] / m_coef[index];
	return arbitraryPoint;
}

void MPlane::Transform(const MMatrix& rMat)
{
	MVector norm = this->Normal();
	MPoint pointOnPlane = this->GetArbitraryPoint();
	MVector newNorm = rMat * norm;
	newNorm.Normalize();
	MPoint newPoint = rMat * pointOnPlane;
	this->Set(newNorm, newPoint);
	return;
}

void MPlane::Write(ostream& os, bool binary) const
{
	if (binary) {
		for(int i=0;i<3;i++)
			os.write((char*)&m_coef[i], sizeof(double));
	}
	else {
		os << m_coef[0] << " " << m_coef[1] << " " << m_coef[2] << " " << m_coef[3] << " ";
	}
}

bool MPlane::Read(ifstream& is, bool binary)
{
	if (binary) {
		for (int i = 0; i < 3; i++) {
			double temp = 0;
			is.read((char*)&temp, sizeof(double));
			m_coef[i] = temp;
		}
		return true;
	}
	return false;
}





