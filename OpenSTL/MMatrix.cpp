#include "pch.h"
#include "MMatrix.h"

/*
	members：
	int m_row;
	int m_col;
	double* m_pData;
	double** m_ppData;
*/

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
	// TODO: 在此处插入 return 语句
}

MMatrix MMatrix::operator+(const MMatrix& mat2)
{
	return MMatrix();
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

	return false;
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

bool MMatrix::LUDecompose(vector<int>& idx)
{
	int i, j, k;
	double sum;
	idx.resize(m_col, 0);
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
		idx[i] = m_i;
		swap(m_ppData[i],m_ppData[m_i]);
		for (j = i; j < m_col; j++)  //计算U[i][j]
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[i][k] * m_ppData[k][j];
			m_ppData[i][j] -= sum;
		}
		for (j = i + 1; j < m_col; j++)//计算L[j][i]
		{
			sum = 0.0;
			for (k = 0; k < i; k++)
				sum += m_ppData[j][k] * m_ppData[k][i];
			m_ppData[j][i] = (m_ppData[j][i] - sum) / m_ppData[i][i];
		}
	}
	return true;
}




