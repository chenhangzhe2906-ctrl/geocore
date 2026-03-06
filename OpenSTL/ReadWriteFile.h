#pragma once
#include"pch.h"


class ReadWriteFile
{
public:
	ReadWriteFile();
	~ReadWriteFile();


public:
	void* ReadSTL(const CString& pathName);
	unsigned long GetFileLength(ifstream& is);
	bool ReadBinary(ifstream& is, vector<MPoint>& points) const;
	bool ReadASCII(const CString& pathName, vector<MPoint>& points) const;
};

