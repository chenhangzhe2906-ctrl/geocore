#pragma once
#include "pch.h"
#include "MPoint.h"

// STL 文件读取（纯 C++ 实现，兼容二进制与 ASCII 格式）
class ReadWriteFile
{
public:
	ReadWriteFile();
	~ReadWriteFile();

public:
	// 读取 STL 文件（自动识别二进制 / ASCII），三角形顶点依次写入 points
	bool ReadSTL(const std::string& pathName, std::vector<MPoint>& points);

	// 返回文件长度
	unsigned long GetFileLength(std::ifstream& is);

	// 读取二进制 STL
	bool ReadBinary(std::ifstream& is, std::vector<MPoint>& points);

	// 读取 ASCII STL
	bool ReadASCII(const std::string& pathName, std::vector<MPoint>& points);
};
