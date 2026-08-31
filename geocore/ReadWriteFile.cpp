#include "pch.h"
#include "ReadWriteFile.h"
#include "MPoint.h"

#include <cstring>

// 二进制 STL 的一个 facet：法向 12B + 3 个顶点 36B + 属性 2B = 50B
struct Facet
{
	float faceNormal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short attribute;
};

ReadWriteFile::ReadWriteFile()
{
}

ReadWriteFile::~ReadWriteFile()
{
}

bool ReadWriteFile::ReadSTL(const std::string& pathName, std::vector<MPoint>& points)
{
	std::ifstream is(pathName, std::ios::binary | std::ios::in);
	if (!is.is_open())
	{
		return false;
	}

	// 跳过 80 字节文件头，读三角形数量，据此判断二进制 / ASCII
	is.seekg(80, std::ios::beg);
	unsigned int nFacet = 0;
	is.read((char*)&nFacet, 4);

	is.seekg(0, std::ios::end);
	std::streamoff fileLen = is.tellg();

	// 二进制 STL 的固定大小：80 头 + 4 数量 + nFacet * 50
	bool binary = (fileLen == 84 + (std::streamoff)nFacet * 50);
	if (binary)
	{
		is.seekg(80, std::ios::beg);          // 回到 facet 数据区
		bool ok = ReadBinary(is, points);
		is.close();
		return ok;
	}

	is.close();
	return ReadASCII(pathName, points);
}

unsigned long ReadWriteFile::GetFileLength(std::ifstream& is)
{
	std::streampos pos = is.tellg();
	is.seekg(0, std::ios::end);
	std::streampos endPos = is.tellg();
	is.seekg(pos, std::ios::beg);
	return (unsigned long)endPos;
}

bool ReadWriteFile::ReadBinary(std::ifstream& is, std::vector<MPoint>& points)
{
	unsigned int nFacet = 0;
	is.seekg(80, std::ios::beg);
	is.read((char*)&nFacet, 4);
	points.reserve(points.size() + (size_t)nFacet * 3);

	Facet facet;
	MPoint p;
	for (unsigned int i = 0; i < nFacet; i++)
	{
		is.read((char*)&facet, sizeof(Facet));
		p.Set(facet.v1[0], facet.v1[1], facet.v1[2]);
		points.push_back(p);
		p.Set(facet.v2[0], facet.v2[1], facet.v2[2]);
		points.push_back(p);
		p.Set(facet.v3[0], facet.v3[1], facet.v3[2]);
		points.push_back(p);
	}
	return true;
}

bool ReadWriteFile::ReadASCII(const std::string& pathName, std::vector<MPoint>& points)
{
	std::ifstream is(pathName);
	if (!is.is_open())
	{
		return false;
	}

	std::vector<MPoint> result;
	std::string line;
	MPoint p;
	while (std::getline(is, line))
	{
		if (line.find("facet normal") == std::string::npos)
		{
			continue;
		}

		// 该 facet 下读取 3 个 vertex（允许中间夹 outer loop 等行）
		int got = 0;
		while (got < 3 && std::getline(is, line))
		{
			size_t s = line.find_first_not_of(" \t\r");
			if (s == std::string::npos)
			{
				continue;
			}
			if (line.compare(s, 6, "vertex") != 0)
			{
				continue;
			}
			std::istringstream ss(line.substr(s));
			std::string tag;
			double x = 0.0, y = 0.0, z = 0.0;
			ss >> tag >> x >> y >> z;
			p.Set(x, y, z);
			result.push_back(p);
			got++;
		}
	}
	is.close();

	if (result.empty())
	{
		return false;
	}
	points.insert(points.end(), result.begin(), result.end());
	return true;
}
