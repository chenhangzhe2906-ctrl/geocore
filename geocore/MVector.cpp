#include "pch.h"
#include "MVector.h"

MVector::MVector(void)
{
	Set(0.0, 0.0, 0.0);
	return;
}

MVector::MVector(double x, double y, double z)
{
	Set(x, y, z);
	return;
}

MVector::~MVector(void){  
}

inline void MVector::Set(double x, double y, double z)  
{
	m_coord[0] = x;
	m_coord[1] = y;
	m_coord[2] = z;
	return;
}

MVector& MVector::operator=(const MVector& vec)
{
	if (this == &vec)
	{
		return *this;
	}
	for (int i = 0; i < 3; i++) {
		this->m_coord[i] = vec.m_coord[i];
	}
	return *this;
}

double MVector::AngleWith(MVector& other)
{
	return static_cast<const MVector&>(*this).AngleWith(other);
}

double MVector::AngleWith(const MVector& other) const
{
	const double eps = 1e-15;
	double mag1 = Magnitude();
	double mag2 = other.Magnitude();
	if (mag1 <= eps || mag2 <= eps)
	{
		return 0.0;
	}
	double dot = (*this % other) / (mag1 * mag2);
	dot = max(-1.0, min(1.0, dot));
	return acos(dot);
}

double MVector::AngleWith(const MVector& vec, const MVector& nor) const
{
	// 在法向量 nor 指定的有向平面内，计算 this 与 vec 的夹角。
	const double eps = 1e-15;
	double mag1 = Magnitude();
	double mag2 = vec.Magnitude();
	if (mag1 <= eps || mag2 <= eps)
	{
		return 0.0;
	}
	MVector n(nor);
	if (n.Magnitude() <= eps)
	{
		return AngleWith(vec);
	}
	n.Normalize();
	double dot = (*this % vec) / (mag1 * mag2);
	dot = max(-1.0, min(1.0, dot));
	double angle = acos(dot);
	if (((*this * vec) % n) < 0.0)
	{
		angle = -angle;
	}
	return angle;
}

void MVector::GetValue(double* d) const
{
	if (d == NULL)
	{
		return;
	}
	d[0] = m_coord[0];
	d[1] = m_coord[1];
	d[2] = m_coord[2];
}

bool MVector::IsParallelTo(const MVector& vec) const
{
	const double eps = 1e-12;
	double len1 = Magnitude();
	double len2 = vec.Magnitude();
	if (len1 <= eps || len2 <= eps)
	{
		return false;
	}
	// 平行包括同向和反向：叉积模长应接近 0。
	return ((*this * vec).Magnitude() <= eps * len1 * len2);
}

double MVector::operator[](int idx) const
{
	return m_coord[idx];
}

double MVector::Magnitude() const  
{
	return sqrt(m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2]);
}

double MVector::Length2() const  
{
	return m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2];
}

void MVector::Normalize()
{
	// FIXED: compute magnitude once. Dividing each component by a magnitude
	// that changes as components are updated produced non-unit results,
	// e.g. (1,1,0) was normalized to (0.707,0.816,0).
	double mag = this->Magnitude();
	if (mag == 0.0)
	{
		return;
	}
	for (int i = 0; i < 3; i++) {
		m_coord[i] /= mag;
	}
	return;
}

bool MVector::IsZero() const  
{
	if (m_coord[0] == 0.0 && m_coord[1] == 0.0 && m_coord[2] == 0.0) {
		return true;
	}
	return false;
}

