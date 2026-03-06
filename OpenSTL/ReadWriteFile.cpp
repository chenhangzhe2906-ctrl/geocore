#include "pch.h"


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
		bool binary = true;
	}
	vector <MPoint> points;
}

unsigned long ReadWriteFile::GetFileLength(ifstream& is)
{
	ULONGLONG pos = is.tellg();
	is.seekg(0, ios::end);
	ULONGLONG endPos = is.tellg();
	is.seekg((long)pos, ios::beg);
	return endPos;
}

bool ReadWriteFile::ReadBinary(ifstream& is, vector<MPoint>& points) const
{
	return false;
}

bool ReadWriteFile::ReadASCII(ifstream& is, vector<MPoint>& points) const
{
	return false;
}


