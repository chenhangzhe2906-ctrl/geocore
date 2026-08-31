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
	double operator[](int idx) const;
	double& operator[](int idx);
	MPoint& operator +=(const MPoint& other) ;
	MPoint& operator -=(const MPoint& other);
	MVector operator-(const MPoint& other) const;
	MVector operator+(const MPoint& other) const;
	friend MPoint operator+(const MPoint& point,const MVector& vec);
	MPoint& operator /=(int a);
	double DistanceToPoint(const MPoint& other) const ;
	friend bool operator==(const MPoint& a, const MPoint& b);

public:
	bool Read(ifstream& is, bool binary /* = true */)
	{
		if (binary)
		{
			is.read((char*)coord, 3 * sizeof(double));
		}
		else
		{
			is >> coord[0] >> coord[1] >> coord[2];
			is.ignore();
		}
		return true;
	}
	void Write(ostream& os, bool binary /* = true */) const
	{
		if (binary)
		{
			os.write((const char*)coord, 3 * sizeof(double));
		}
		else
		{
			os << coord[0] << " " << coord[1] << " " << coord[2] << endl;
		}
	}


private:
	double coord[3];
};
