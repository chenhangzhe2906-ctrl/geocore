#pragma once
#include "pch.h"
#include"MVector.h"

class MPoint {
public:
	MPoint();
	~MPoint();
	MPoint(double x, double y, double z);
	void Set(double x, double y, double z);
	MPoint& operator=(const MPoint& point);
	double operator[](int idx) const ;
	MPoint& operator +=(const MPoint& other) ;
	MPoint& operator -=(const MPoint& other);
	MVector& operator-(const MPoint& other) const;
	MVector& operator+(const MPoint& other) const;
	friend MPoint operator+(const MPoint& point,const MVector& vec);
	MPoint& operator /=(int a);
	double DistanceToPoint(const MPoint& other) const ;
	friend bool operator==(const MPoint& a, const MPoint& b);

private:
	double coord[3];
};
