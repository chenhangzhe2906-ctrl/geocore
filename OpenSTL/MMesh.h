#pragma once
#include "pch.h"
#include"MPoint.h"
#include"MLine.h"
#include "MObject.h"

struct Triangle {
	int coord[3];
	Triangle() {
		coord[0] = coord[1] = coord[2] = -1;
	}
	Triangle(int a, int b, int c) {
		coord[0] = a;
		coord[1] = b;
		coord[2] = c;
	}
	bool IsValid() {
		return (coord[0]==coord[1] && coord[0] == coord[2] && coord[2] == coord[1]);
	}
};

struct TcEdgePoint
{
public:
	TcEdgePoint(const MPoint& pos, shared_ptr <MObject>& pRelyObj)
	{
		m_pos = pos;
		m_pRelyObj = pRelyObj;
	}

public:
	const MPoint& GetPos() const { return m_pos; }
	const shared_ptr<MObject> GetRelyObj() const { return m_pRelyObj; }

private:
	shared_ptr <MObject> m_pRelyObj;	// TcEdge or TcVertex
	MPoint m_pos;
};

class MMesh
{

};

