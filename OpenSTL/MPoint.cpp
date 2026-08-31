#include "pch.h"
#include "MPoint.h"
#include "MVector.h"

MPoint::MPoint()
{
	Set(0.0, 0.0, 0.0);
}

MPoint::~MPoint()
{

}

MPoint::MPoint(double x, double y, double z)
{
	Set(x, y, z);
	return;
}

void MPoint::Set(double x, double y, double z)
{
	this->coord[0]= x;
	this->coord[1] = y;
	this->coord[2] = z;
	return;
}

MPoint & MPoint::operator=(const MPoint& point)
{
	if (this != &point) {
		coord[0] = point.coord[0];
		coord[1] = point.coord[1];
		coord[2] = point.coord[2];
	}
	return *this;
}

double MPoint::operator[](int idx) const 
{
	return this->coord[idx];
}

double& MPoint::operator[](int idx)
{
	return this->coord[idx];
}

MPoint& MPoint::operator+=(const MPoint& other)
{
	for (int i = 0; i < 3; i++) {
		this->coord[i] += other.coord[i];
	}
	return *this;
}

MPoint& MPoint::operator-=(const MPoint& other)
{
	for (int i = 0; i < 3; i++) {
		this->coord[i] -= other.coord[i];
	}
	return *this;
}

MVector MPoint::operator-(const MPoint& other) const
{
	// 修正：原实现 new 一个 MVector 再返回引用，每次调用都泄漏内存；改为按值返回
	return MVector(this->coord[0] - other.coord[0],
		this->coord[1] - other.coord[1],
		this->coord[2] - other.coord[2]);
}

MVector MPoint::operator+(const MPoint& other) const
{
	return MVector(this->coord[0] + other.coord[0],
		this->coord[1] + other.coord[1],
		this->coord[2] + other.coord[2]);
}

MPoint& MPoint::operator/=(int a)
{
	for (int i = 0; i < 3; i++) {
		this->coord[i] /= a;
	}
	return *this;
}

double MPoint::DistanceToPoint(const MPoint& other) const
{
	// 修正：原实现返回平方距离，与函数名不符；改为返回真实欧氏距离
	double dx = this->coord[0] - other.coord[0];
	double dy = this->coord[1] - other.coord[1];
	double dz = this->coord[2] - other.coord[2];
	return sqrt(dx * dx + dy * dy + dz * dz);
}

MPoint operator+(const MPoint& point, const MVector& vec)
{
	return MPoint(point[0] + vec[0], point[1] + vec[1], point[2] + vec[2]);
}

bool operator==(const MPoint& a, const MPoint& b)
{
	for (int i = 0; i < 3; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}


