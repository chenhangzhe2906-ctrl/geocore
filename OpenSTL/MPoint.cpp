#include "pch.h"
#include "MPoint.h"

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
