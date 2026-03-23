#include "pch.h"
#include "MMatrix.h"
#include"MPoint.h"

/*
	members：
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
}

MMatrix::MMatrix(int r, int c)
{
	this->m_row = r;
	this->m_col = c;
	this->m_pData = new double[r * c];
	this->m_ppData = new double*[r];
	for (int i = 0; i < r; i++) {
		m_ppData[i] = &m_pData[i*c];
	}
	memset(m_pData, 0.0, sizeof(double) * r * c);
	return;
}

MMatrix::MMatrix(const MMatrix& mat)
{
	this->m_col = mat.m_col;
	this->m_pData = mat.m_pData;
	this->m_ppData = mat.m_ppData;
	this->m_row = mat.m_row;
}

MMatrix::~MMatrix(void)
{
	delete m_pData;
	delete m_ppData;
}

MMatrix& MMatrix::operator=(const MMatrix& mat)
{
	if (this == &mat) return *this;
	this->m_col = mat.GetCol();
	this->m_row = mat.GetRow();
	delete[] m_pData;
	delete[] m_ppData;
	m_pData = new double[m_col * m_row];
	m_ppData = new double*[m_row];
	for (int i = 0; i < m_row*m_col; i++) {
		m_pData[i] = mat.m_pData[i];
	}
	for (int j = 0; j < m_row; j++) {
		m_ppData[j] = &m_pData[j * m_col];
	}
	return *this;
}

MMatrix MMatrix::operator+(const MMatrix& mat2)
{
	for (int i = 0; i < m_row; i++) {
		for (int j = 0; j < m_col; j++) {
			(*this)[i][j] += mat2[i][j];
		}
	}
	return *this;
}

double* MMatrix::operator[](int r) const
{
	return m_ppData[r];
}

void MMatrix::Set(int r, int c, double* pData)
{
	m_row = r;
	m_col = c;
	m_pData = pData;
	for (int i = 0; i < r; i++) {
		m_ppData[i] = &m_pData[i * c];
	}
	return;
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

bool MMatrix::Read(ifstream is, bool binary)
{
	if(binary)
	{
		is.read((char*)&m_row, sizeof(int));
		is.read((char*)&m_col, sizeof(int));
	}
	else
	{
		is >> m_row >> m_col;
		is.ignore();
	}

	if (m_row == 0 || m_col == 0)
	{
		return true;
	}

	Set(m_row, m_col);

	if (binary)
	{
		is.read((char*)m_pData, m_row * m_col * sizeof(double));
	}
	else
	{
		for (int i = 0; i < m_col * m_row; i++)
		{
			is >> m_pData[i];
		}
		is.ignore();
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
	for (int i = 0; i < m_row; i++) {
		for (int j = 0; j < m_col; j++) {
			if (i == j && fabs((*this)[i][j]-1.0)>ERR8) {
				return  false;
			}
			else if (fabs((*this)[i][j]) > ERR8) {
				return false;
			}
		}
	}
	return true;
}

//采用循环 Jacobi（Cyclic Jacobi）; hypot避免溢出;减少对称矩阵的冗余更新
bool MMatrix::JacobiEigen(double valEigen[3], MMatrix& vecEigen) const
{
	int i, j;
	int nMaxIter = 60;
	const double epsilon = 1e-6;
	while (1) {
		for (i = 0; i < m_col; i++) {
			for (j = 0; j < m_row; j++) {
				if (i != j && fabs((*this)[i][j]) > epsilon) {
					
				}
			}
		}
	}
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
		for (j = i; j < m_col; j++)//计算U[i][j]
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
	iota(idx.begin(), idx.end(), 0);  // 得到 [0,1,2,...,m_row-1]
	const double epsilon = 1e-8;
	for (i = 0; i < m_row; i++)
	{
		double m = epsilon;
		int m_i = i;
		for (j = i; j < m_row; j++) {
			if (fabs((*this)[j][i]) > fabs(m)) {
				m = (*this)[j][i];
				m_i = j;
			}
		}
		if (fabs(m) <= epsilon) {
			return false;
		}
		if (m_i != i) {
			std::swap(m_ppData[i], m_ppData[m_i]); // 矩阵行交换
			std::swap(idx[i], idx[m_i]);           // 置换同步
		}
		for (j = i; j < m_col; j++)  //计算U[i][j]
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[i][k] * m_ppData[k][j];
			m_ppData[i][j] -= sum;
		}
		for (j = i + 1; j < m_row; j++)//计算L[j][i]
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[j][k] * m_ppData[k][i];
			m_ppData[j][i] = (m_ppData[j][i] - sum) / m_ppData[i][i];
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
		b[i] = (y[i] - sum) / piv;   // 解写回 b
	}
	return true;
}





