// pch.h：预编译标头
// 本项目为纯 C++ 几何库，不再依赖 MFC / Windows 头文件。

#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <istream>
#include <ostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <memory>

using namespace std;

#define ERR8 1e-8
#define ERR7 1e-7

// 兼容：原 MFC 工程里 ASSERT 来自 MFC，纯 C++ 环境下退化为标准 assert
#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

#endif //PCH_H
