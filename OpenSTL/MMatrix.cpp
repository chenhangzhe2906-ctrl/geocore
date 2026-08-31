#include "pch.h"
#include "MMatrix.h"
#include"MPoint.h"

/*
	members:
	int m_row;
	int m_col;
	double* m_pData;
	double** m_ppData;
*/

MMatrix MMatrix::s_GetRotationMatrix(const MVector& vec, double angle)
{
	MMatrix mat(3, 3);
	if (vec.IsZero())
	{
		mat.SetIdentity();
		return mat;
	}
	MVector utVec(vec);
	utVec.Normalize();
	double x = utVec[0];
	double y = utVec[1];
	double z = utVec[2];
	double c = cos(angle);
	double s = sin(angle);
	double t = 1.0 - c;	double tx = t * x;
	double ty = t * y;
	double tz = t * z;
	double sx = s * x;
	double sy = s * y;
	double sz = s * z;
	//-----------------------------------------------------------
	//		| t*x*x + c		t*x*y + s*z		t*x*z - s*y |
	//		|											|
	//	R = | t*x*y - s*z	t*y*y + c		t*y*z + s*x |
	//		|											|
	//		| t*x*z + s*y	t*y*z - s*x		t*z*z + c	|
	//
	// where c = cos(theta), s = sin(theta), t = 1 - c and(x, y, z) is a unit
	// vector on the axis of rotation.
	//-----------------------------------------------------------

	// row one
	mat[0][0] = tx * x + c;
	mat[0][1] = tx * y - sz;
	mat[0][2] = tx * z + sy;

	// row two
	mat[1][0] = tx * y + sz;
	mat[1][1] = ty * y + c;
	mat[1][2] = ty * z - sx;

	// row third
	mat[2][0] = tx * z - sy;
	mat[2][1] = ty * z + sx;
	mat[2][2] = tz * z + c;

	return	mat;

}

MMatrix MMatrix::s_GetRotationMatrix(const MVector& srcVec, const MVector& dstVec)
{
	MVector normal = srcVec * dstVec;
	if (normal.IsZero())
	{
		MMatrix mat(3, 3);
		mat.SetIdentity();

		double dot = srcVec % dstVec;
		if (dot < 0)
		{
			mat[2][2] = -1.0;
		}

		return mat;
	}
	normal.Normalize();
	double angle = srcVec.AngleWith(dstVec);
	return s_GetRotationMatrix(normal, angle);
}

MMatrix::MMatrix(void)
{
	m_row = 0;
	m_col = 0;
	m_pData = NULL;
	m_ppData = NULL;
}

MMatrix::MMatrix(int r, int c)
	: m_row(r), m_col(c), m_pData(NULL), m_ppData(NULL)
{
	if (r <= 0 || c <= 0)
	{
		m_row = 0;
		m_col = 0;
		return;
	}
	m_pData = new double[static_cast<size_t>(r) * static_cast<size_t>(c)]();
	m_ppData = new double*[r];
	for (int i = 0; i < r; i++) {
		m_ppData[i] = &m_pData[static_cast<size_t>(i) * c];
	}
}

MMatrix::MMatrix(const MMatrix& mat)
{
	// FIXED: the original copy ctor only copied pointers, and the destructor
	// freed them, causing double-free (crashed in unoptimized Debug builds).
	// Now performs a proper deep copy.
	this->m_col = mat.m_col;
	this->m_row = mat.m_row;
	m_pData = new double[m_row * m_col];
	m_ppData = new double*[m_row];
	for (int i = 0; i < m_row * m_col; i++)
	{
		m_pData[i] = mat.m_pData[i];
	}
	for (int j = 0; j < m_row; j++)
	{
		m_ppData[j] = &m_pData[static_cast<size_t>(j) * static_cast<size_t>(m_col)];
	}
}

MMatrix::~MMatrix(void)
{
	delete[] m_pData;
	delete[] m_ppData;
}

MMatrix& MMatrix::operator=(const MMatrix& mat)
{
	if (this == &mat) return *this;

	if (mat.m_row <= 0 || mat.m_col <= 0)
	{
		delete[] m_pData;
		delete[] m_ppData;
		m_row = 0;
		m_col = 0;
		m_pData = NULL;
		m_ppData = NULL;
		return *this;
	}

	MMatrix temp(mat);  // 先深拷贝，保证自分配异常时原对象不被破坏
	std::swap(m_row, temp.m_row);
	std::swap(m_col, temp.m_col);
	std::swap(m_pData, temp.m_pData);
	std::swap(m_ppData, temp.m_ppData);
	return *this;
}

MMatrix MMatrix::operator+(const MMatrix& mat2)
{
	if (m_row != mat2.m_row || m_col != mat2.m_col)
	{
		return MMatrix();
	}
	MMatrix result(m_row, m_col);
	for (int i = 0; i < m_row; i++) {
		for (int j = 0; j < m_col; j++) {
			result[i][j] = (*this)[i][j] + mat2[i][j];
		}
	}
	return result;
}

double* MMatrix::operator[](int r) const
{
	return m_ppData[r];
}

void MMatrix::Set(int r, int c, double* pData)
{
	if (r < 0 || c < 0)
	{
		return;
	}
	delete[] m_pData;
	delete[] m_ppData;
	m_row = r;
	m_col = c;
	m_pData = (pData != NULL) ? pData : ((r > 0 && c > 0) ? new double[r * c]() : NULL);
	m_ppData = (r > 0) ? new double*[r] : NULL;
	for (int i = 0; i < r; i++) {
		m_ppData[i] = &m_pData[static_cast<size_t>(i) * static_cast<size_t>(c)];
	}
}

void MMatrix::Write(ostream& os, bool binary) const
{
	if (binary)
	{
		os.write((const char*)&m_row, sizeof(int));
		os.write((const char*)&m_col, sizeof(int));
		os.write((const char*)m_pData, m_row * m_col * sizeof(double));
	}
	else
	{
		os << m_row << " " << m_col << endl;
		for (int i = 0; i < m_row; i++)
		{
			for (int j = 0; j < m_col; j++)
			{
				os << left;
				os << fixed << m_ppData[i][j] << " ";
			}
			os << endl;
		}
	}
}

bool MMatrix::Read(ifstream& is, bool binary)
{
	int row = 0;
	int col = 0;
	if (binary)
	{
		if (!is.read((char*)&row, sizeof(int)) || !is.read((char*)&col, sizeof(int)))
		{
			return false;
		}
	}
	else
	{
		if (!(is >> row >> col))
		{
			return false;
		}
	}

	if (row < 0 || col < 0)
	{
		return false;
	}
	Set(row, col);
	if (row == 0 || col == 0)
	{
		return true;
	}

	if (binary)
	{
		return (bool)is.read((char*)m_pData, (std::streamsize)row * col * sizeof(double));
	}
	for (int i = 0; i < row * col; i++)
	{
		if (!(is >> m_pData[i]))
		{
			return false;
		}
	}
	return true;
}

void MMatrix::SetIdentity()
{
	for (int i = 0; i < m_row; i++) {
		for(int j = 0; j < m_col; j++) {
			if (i == j) {
				(*this)[i][j] = 1.0;
			}
			else {
				(*this)[i][j] = 0.0;
			}
		}
	}
	return;
}

bool MMatrix::IsIdentity() const
{
	// FIXED: the original else-if was chained to the diagonal branch, so an
	// identity matrix's diagonal value 1 was treated as "non-zero and failed",
	// and IsIdentity() always returned false.
	for (int i = 0; i < m_row; i++) {
		for (int j = 0; j < m_col; j++) {
			if (i == j) {
				if (fabs((*this)[i][j] - 1.0) > ERR8) {
					return false;
				}
			}
			else {
				if (fabs((*this)[i][j]) > ERR8) {
					return false;
				}
			}
		}
	}
	return true;
}

 // 循环 Jacobi（Cyclic Jacobi）求对称矩阵特征值
bool MMatrix::JacobiEigen(double valEigen[3], MMatrix& vecEigen) const
{
	
	if (m_row != 3 || m_col != 3)
	{
		return false;
	}

	// 工作矩阵：深拷贝一份，避免修改自身
	MMatrix A(3, 3);
	A = *this;

	// 特征向量矩阵，初始为单位阵；收敛后各列即特征向量
	MMatrix V(3, 3);
	V.SetIdentity();

	const int maxIter = 100;
	const double eps = 1e-12;

	int iter = 0;
	while (iter++ < maxIter)
	{
		// 找最大的非对角元 (p, q)
		int p = 0, q = 1;
		double maxOff = 0.0;
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 3; j++)
			{
				double a = fabs(A[i][j]);
				if (a > maxOff)
				{
					maxOff = a;
					p = i;
					q = j;
				}
			}
		}
		if (maxOff < eps)
		{
			break;   // 已收敛
		}

		// 旋转角：tan(2θ) = 2·A[p][q] / (A[q][q] - A[p][p])
		double theta = 0.5 * atan2(2.0 * A[p][q], A[q][q] - A[p][p]);
		double c = cos(theta);
		double s = sin(theta);

		// A' = J^T · A · J，分两步：
		//   1) 左乘 J^T：旋转第 p、q 行（用旧行值）
		double a_pk[3], a_qk[3];
		for (int k = 0; k < 3; k++)
		{
			a_pk[k] = A[p][k];
			a_qk[k] = A[q][k];
		}
		for (int k = 0; k < 3; k++)
		{
			A[p][k] = c * a_pk[k] - s * a_qk[k];
			A[q][k] = s * a_pk[k] + c * a_qk[k];
		}
		//   2) 右乘 J：旋转第 p、q 列（读当前值，写新值，保持对称）
		for (int i = 0; i < 3; i++)
		{
			double m_ip = A[i][p];
			double m_iq = A[i][q];
			A[i][p] = c * m_ip - s * m_iq;
			A[i][q] = s * m_ip + c * m_iq;
		}

		// 累积特征向量：V = V · J
		for (int k = 0; k < 3; k++)
		{
			double vkp = V[k][p], vkq = V[k][q];
			V[k][p] = c * vkp - s * vkq;
			V[k][q] = s * vkp + c * vkq;
		}
	}

	// 收敛后对角线元素即特征值
	for (int i = 0; i < 3; i++)
	{
		valEigen[i] = A[i][i];
	}
	vecEigen = V;   // 特征向量按列存放
	return true;
}

bool MMatrix::Inverse()
{
	//resolve by converting it into n linear system
	MMatrix mat(*this);
	vector<int> idx(mat.GetRow());
	if (!mat.LUDecompose(idx))
	{
		return false;
	}
	double* b = new double[m_row];
	for (int i = 0; i < m_row; i++)
	{
		memset(b, 0, sizeof(double) * m_row);
		b[i] = 1.0;
		bool bSuc = mat.GetSolutionAfterLUDecompose(idx, b);
		for (int j = 0; j < m_row; j++)
		{
			m_ppData[j][i] = b[j];
		}
	}
	delete[] b;
	return true;
}

void MMatrix::Transpose()
{
	MMatrix Mat(m_col, m_row);
	for (int i = 0; i < m_col; i++)
	{
		for (int j = 0; j < m_row; j++)
		{
			Mat[i][j] = m_ppData[j][i];
		}
	}
	*this = Mat;
}

bool MMatrix::LUDecompose()
{
	int i, j, k;
	double sum;
	for (i = 0; i < m_row; i++)
	{
		for (j = i; j < m_col; j++) // 计算 U[i][j]
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[i][k] * m_ppData[k][j];
			m_ppData[i][j] -= sum;
		}
		for (j = i + 1; j < m_col; j++)
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[j][k] * m_ppData[k][i];
			if (fabs(m_ppData[i][i]) < 1.0e-9)
				return false;
			m_ppData[j][i] = (m_ppData[j][i] - sum) / m_ppData[i][i];
		}
	}
	return true;
}

bool MMatrix::GetSolutionAfterLUDecompose(double* b, double* x)
{
	double sum = 0.0;
	auto z = std::make_unique<double[]>(m_row);      
	for (int i = 0; i < m_row; i++)
	{
		sum = 0.0;
		for (int j = 0; j < i; j++)
			sum += m_ppData[i][j] * z[j];
		z[i] = b[i] - sum;
	}

	for (int i = m_row - 1; i >= 0; i--)
	{
		if (fabs(m_ppData[i][i]) < 1.0e-9)
		{
			return false;
		}

		sum = 0.0;
		for (int j = i + 1; j < m_col; j++)
			sum += m_ppData[i][j] * x[j];

		x[i] = (z[i] - sum) / m_ppData[i][i];
	}
	return true;
}

bool MMatrix::GetSolutionAfterLUDecompose(const vector<MPoint>& b, vector<MPoint>& root)
{
	for (int k = 0; k < 3; k++)
	{
		double sum = 0.0;
		//double* z = new double[m_row];
		auto z = std::make_unique<double[]>(m_row);
		for (int i = 0; i < m_row; i++)
		{
			sum = 0.0;
			for (int j = 0; j < i; j++)
				sum += m_ppData[i][j] * z[j];
			z[i] = b[i][k] - sum;
		}
		for (int i = m_row - 1; i >= 0; i--)
		{
			if (fabs(m_ppData[i][i]) < 1.0e-9)
			{
				return false;
			}

			sum = 0.0;
			for (int j = i + 1; j < m_col; j++)
				sum += m_ppData[i][j] * root[j][k];
			root[i][k] = (z[i] - sum) / m_ppData[i][i];
		}
	}

	return true;
}

MMatrix MMatrix::GetNormalMatrix(int* notZeroStart, int* notZeroEnd, double* fabsMax) const
{
	bool bRelStart = false;
	bool bRelEnd = false;

	if (notZeroStart == NULL)
	{
		notZeroStart = new int[m_col];
		memset(notZeroStart, 0, sizeof(int) * m_col);

		bRelStart = true;
	}

	if (notZeroEnd == NULL)
	{
		notZeroEnd = new int[m_col];
		for (int i = 0; i < m_col; i++)
		{
			notZeroEnd[i] = m_row - 1;
		}

		bRelEnd = true;
	}

	MMatrix N(m_col, m_col);
	double maxValue = 0.0;
	for (int i = 0; i < m_col; i++)
	{
		for (int j = 0; j < m_col; j++)
		{
			int start = max(notZeroStart[i], notZeroStart[j]);
			int end = min(notZeroEnd[i], notZeroEnd[j]);

			for (int k = start; k <= end; k++)
			{
				N[i][j] += m_ppData[k][i] * m_ppData[k][j];
			}

			maxValue = max(maxValue, fabs(N[i][j]));
		}
	}

	if (bRelStart)
	{
		delete [] notZeroStart;
	}

	if (bRelEnd)
	{
		delete[] notZeroEnd;
	}

	if (fabsMax)
	{
		*fabsMax = maxValue;
	}

	return N;
}

void MMatrix::ToArray(double mat[16]) const
{
	memset(mat, 0, sizeof(double) * 16);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat[4 * i + j] = m_ppData[j][i];
		}
	}
}

bool MMatrix::LUDecompose(vector<int>& idx)
{
	
	int i, j, k;
	double sum;
	idx.resize(m_col);
	idx.resize(m_row);
	iota(idx.begin(), idx.end(), 0);   // 得到初始置换 [0,1,2,...,m_row-1]
	const double epsilon = 1e-8;
	for (i = 0; i < m_row; i++)
	{
		// 先把第 i 列（含对角线）更新为 Schur 补：A[j][i] -= Σ_{k<i} A[j][k]*A[k][i]
		for (j = i; j < m_row; j++)
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[j][k] * m_ppData[k][i];
			m_ppData[j][i] -= sum;
		}

		// 基于更新后的列选主元
		double m = epsilon;
		int m_i = i;
		for (j = i; j < m_row; j++) {
			if (fabs(m_ppData[j][i]) > fabs(m)) {
				m = m_ppData[j][i];
				m_i = j;
			}
		}
		if (fabs(m) <= epsilon) {
			return false;
		}
		if (m_i != i) {
			std::swap(m_ppData[i], m_ppData[m_i]);  // 交换两行
			std::swap(idx[i], idx[m_i]);            // 置换同步
		}

		for (j = i + 1; j < m_col; j++)
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[i][k] * m_ppData[k][j];
			m_ppData[i][j] -= sum;
		}

		// 计算 L 列
		for (j = i + 1; j < m_row; j++)
		{
			m_ppData[j][i] /= m_ppData[i][i];
		}
	}
	return true;
}

/*bool MMatrix::GetSolutionAfterLUDecompose(const vector<int>& idx, double* b)
{
	double sum = 0.0;
	auto z = std::make_unique<double[]>(m_row);
	auto x = std::make_unique<double[]>(m_row);
	int length = _msize(b) / sizeof(double);
	if (m_col != m_row || length != m_col) {
		return false;
	}
	double* temp_b = new double[m_col];
	memcpy(temp_b, b, sizeof(double) * m_col);
	for (int i = 0; i < m_row; i++) {
		b[i] = temp_b[idx[i]];
	}
	delete[] temp_b;
	for (int i = 0; i < m_row; i++)
	{
		sum = 0.0;
		for (int j = 0; j < i; j++)
			sum += m_ppData[i][j] * z[j];
		z[i] = b[i] - sum;
	}
	for (int i = m_row - 1; i >= 0; i--)
	{
		if (fabs(m_ppData[i][i]) < 1.0e-9)
		{
			return false;
		}
		sum = 0.0;
		for (int j = i + 1; j < m_col; j++)
			sum += m_ppData[i][j] * x[j];
		x[i] = (z[i] - sum) / m_ppData[i][i];
	}
	double* b = x.release();
	return true;
}*/

bool MMatrix::GetSolutionAfterLUDecompose(const vector<int>& idx, double* b) {
	if (!b || m_row != m_col) return false;
	int n = m_row;
	const double eps = 1e-9;

	std::vector<double> y(n);
	for (int i = 0; i < n; ++i) y[i] = b[idx[i]];       // P b

	 // 前代：L y = P b
	for (int i = 0; i < n; ++i) {
		double sum = 0;
		for (int j = 0; j < i; ++j) sum += m_ppData[i][j] * y[j];
		y[i] -= sum;
	}

	 // 回代：U x = y
	for (int i = n - 1; i >= 0; --i) {
		double sum = 0;
		for (int j = i + 1; j < n; ++j) sum += m_ppData[i][j] * b[j];
		double piv = m_ppData[i][i];
		if (std::fabs(piv) < eps) return false;
		b[i] = (y[i] - sum) / piv;    // 写回 b
	}
	return true;
}





