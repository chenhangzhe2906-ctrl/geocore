#pragma once
#include "pch.h"
#include "MVector.h"
#include "MPoint.h"
#include"MLine.h"

class MMatrix
{
public:
	static MMatrix s_GetRotationMatrix(const MVector& vec, double angle);
	static MMatrix s_GetRotationMatrix(const MVector& srcVec, const MVector& dstVec);

public:
	MMatrix(void);
	MMatrix(int r, int c);
	MMatrix(const MMatrix& mat);
	~MMatrix(void);

public:
	MMatrix& operator =(const MMatrix& mat);
	MMatrix operator+(const MMatrix& mat2);
	double* operator [](int r) const;
	friend MVector operator*(const MMatrix& mat, const MVector& vec);
	friend MPoint operator*(const MMatrix& mat, const MPoint& point);


	int GetRow() const { return m_row; }
	int GetCol() const { return m_col; }
	void Set(int r, int c, double* pData = NULL);

public:
	void Write(ostream& os, bool binary = true) const;
	bool Read(ifstream& is, bool binary = true);

	void SetIdentity();
	bool IsIdentity() const;
	bool JacobiEigen(double valEigen[3], MMatrix& vecEigen) const;

	bool Inverse(); // 原地求逆（失败返回 false）
	void Transpose(); // 转置

	bool LUDecompose();// LU decompose matrix, used for resolve liner system, Ax = b
	bool GetSolutionAfterLUDecompose(double* b, double* x);
	bool GetSolutionAfterLUDecompose(const vector<MPoint>& b, vector<MPoint>& root);

	bool LUDecompose(vector<int>& idx);
	bool GetSolutionAfterLUDecompose(const vector<int>& idx, double* b);
	bool GetSolutionAfterLUDecompose(const vector<int>& idx, vector<MPoint>& b);

	MMatrix GetNormalMatrix(int* notZeroStart = NULL, int* notZeroEnd = NULL, double* fabsMax = NULL) const;
	void ToArray(double mat[16]) const;

public:
	friend MMatrix operator *(const MMatrix& mat1, const MMatrix& mat2);
	MMatrix operator *(double dScalar) const;
	friend bool operator != (const MMatrix& mat1, const MMatrix& mat2);


private:
	int m_row;
	int m_col;
	double* m_pData;
	double** m_ppData;
};

inline MVector operator*(const MMatrix& mat, const MVector& vec)
{
	vector <double> temp;
	temp.reserve(3);
	for (int i = 0; i < 3; i++) {
		double t = 0.0;
		for (int j = 0; j < mat.m_row; j++) {
			t += mat[i][j] * vec[j];   // FIXED: was vec[i], which broke mat*vec
		}
		temp.push_back(t);
	}
	return MVector(temp[0], temp[1], temp[2]);
}

inline MPoint operator*(const MMatrix& mat, const MPoint& point)
{
	vector <double> temp;
	temp.reserve(3);
	for (int i = 0; i < 3; i++) {
		double t = 0.0;
		for (int j = 0; j < mat.m_row; j++) {
			t += mat[i][j] * point[j];   // FIXED: was point[i], which broke mat*point
		}
		temp.push_back(t);
	}
	return MPoint(temp[0], temp[1], temp[2]);
}

inline MMatrix operator *(const MMatrix& mat1, const MMatrix& mat2)
{
	//ASSERT(mat1.GetCol() == mat2.GetRow());

	MMatrix result(mat1.GetRow(), mat2.GetCol());

	for (int i = 0; i < mat1.GetRow(); i++)
	{
		for (int j = 0; j < mat2.GetCol(); j++)
		{
			double sum = 0.0;
			for (int n = 0; n < mat1.GetCol(); n++)
			{
				sum += mat1[i][n] * mat2[n][j];
			}

			result[i][j] = sum;
		}
	}

	return result;
}

inline MMatrix MMatrix::operator *(double dScalar) const
{
	MMatrix kProd(m_row, m_col);
	for (int iRow = 0; iRow < m_row; iRow++)
	{
		for (int iCol = 0; iCol < m_col; iCol++)
			kProd[iRow][iCol] = dScalar * m_ppData[iRow][iCol];
	}
	return kProd;
}
inline bool operator !=(const MMatrix& mat1, const MMatrix& mat2)
{
	if ((mat1.GetRow() != mat2.GetRow()) || (mat1.GetCol() != mat2.GetCol()))
	{
		return true;
	}
	for (int i = 0; i < mat1.GetRow(); i++)
	{
		for (int j = 0; j < mat1.GetCol(); j++)
		{
			if (mat1[i][j] != mat2[i][j])
			{
				return true;
			}
		}
	}
	return false;

}

