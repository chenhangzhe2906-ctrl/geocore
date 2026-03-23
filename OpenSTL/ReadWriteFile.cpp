#include "pch.h"
#include"ReadWriteFile.h"
#include"MPoint.h"

struct Facet
{
	float faceNormal[3]; //法向量
	float v1[3];//坐标
	float v2[3];//坐标
	float v3[3];//坐标
	UINT16 attribute;//附加属性
};

ReadWriteFile::ReadWriteFile()
{
}

ReadWriteFile::~ReadWriteFile()
{
}

void* ReadWriteFile::ReadSTL(const CString& pathName)
{
	ifstream is(pathName, ios::binary | ios::in);
	unsigned long long fileLength=0;
	unsigned int nFacet = 0;
	if (!is.is_open())
	{
		return NULL;
	}
	is.seekg(80, ios::beg);
	is.read((char*)&nFacet, 4);
	bool binary = false;
	fileLength = GetFileLength(is);
	if (fileLength == 84 + nFacet * 50) {
		binary = true;
	}
	vector <MPoint> points;
	bool status = false;
	if (binary == true) {
		status = ReadBinary(is, points);
		is.close();
	}
	else {
		is.close();
		status = ReadASCII(pathName, points);
	}

}

unsigned long ReadWriteFile::GetFileLength(ifstream& is)
{
	ULONGLONG pos = is.tellg();
	is.seekg(0, ios::end);
	ULONGLONG endPos = is.tellg();
	is.seekg((long)pos, ios::beg);
	return endPos;
}

bool ReadWriteFile::ReadBinary(ifstream& is, vector<MPoint>& points)
{
	unsigned int nFacet = 0;
	is.seekg(80, ios::beg);
	is.read((char*)&nFacet, 4);
	points.reserve(nFacet * 3);
	Facet facet;
	MPoint p;
	for (int i = 0; i < nFacet; i++) {
		is.read((char*)&facet, 50);
		p.Set(facet.v1[0], facet.v1[1], facet.v1[2]);
		points.push_back(p);
		p.Set(facet.v2[0], facet.v2[1], facet.v2[2]);
		points.push_back(p);
		p.Set(facet.v3[0], facet.v3[1], facet.v3[2]);
		points.push_back(p);
	}

	return true;
}

bool ReadWriteFile::ReadASCII(const CString& pathName, vector<MPoint>& points)
{
	CStdioFile MyFile;
	MPoint p;
	if (!MyFile.Open(pathName, CFile::modeRead | CFile::typeText)) {
		return false;
	}
	if (MyFile.GetLength() == 0) {
		MyFile.Close();
		return false;
	}
	CString Line;
	while (1) {
		if (Line.Find(_T("facet normal")) == -1) {
			break;
		}
		for (int i = 0; i < 3; i++) {
			double x, y, z;
			MyFile.ReadString(Line);
			swscanf_s(Line, _T("vertex %lf%lf%lf"), &x, &y, &z);
			p.Set(x, y, z);
			points.push_back(p);
		}
		MyFile.ReadString(Line);	// 跳过end loop
		MyFile.ReadString(Line);	//跳过 endfacet
	}
	if (points.size() == 0)
	{
		MyFile.Close();
		return false;
	}
	MyFile.Close();
	return true;
}


