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

MVector& MPoint::operator-(const MPoint& other) const
{
	double newCoord[3];
	for (int i = 0; i < 3; i++) {
		newCoord[i]=this->coord[i] - other.coord[i];
	}
	MVector *newVec = new MVector(newCoord[0], newCoord[1], newCoord[2]);
	return *newVec;
}

MVector& MPoint::operator+(const MPoint& other) const
{
	double newCoord[3];
	for (int i = 0; i < 3; i++) {
		newCoord[i] = this->coord[i] + other.coord[i];
	}
	MVector* newVec = new MVector(newCoord[0], newCoord[1], newCoord[2]);
	return *newVec;
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
	return (this->coord[0] - other.coord[0]) * (this->coord[0] - other.coord[0]) + (this->coord[1] - other.coord[1]) * (this->coord[1] - other.coord[1]) + (this->coord[2] - other.coord[2]) * (this->coord[2] - other.coord[2]);
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


