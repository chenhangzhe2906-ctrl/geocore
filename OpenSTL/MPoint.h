#pragma once
#include "pch.h"

class MPoint {
public:
	MPoint();
	~MPoint();
	MPoint(double x, double y, double z);
	void Set(double x, double y, double z);
	MPoint& operator=(const MPoint& point);
	double operator[](int idx) const ;

private:
	double coord[3];
};
