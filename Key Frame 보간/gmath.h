#ifndef _GMATH_H
#define _GMATH_H

#include <vector>
#include <iostream>
#include <cmath>
#include <cassert>
#include <iomanip>
#include <cstdarg>

// ��ũ�� ���
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616

// ��ũ�� ����
#define SQRT(X)		sqrt((X))
#define SQR(X)		((X) * (X))
#define DEG2RAD(X)	((X) * (M_PI) / (180.0))
#define RAD2DEG(X)	((X) * (180.0) / (M_PI))
#define SWAP(type, x, y) { type temp = (x); (x) = (y); (y) = temp; }
#define MIN(x, y)	((x) > (y) ? (y) : (x))
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define ABS(X)		(((X) > 0.0) ? (X) : (-(X)))
#define SIGN(a)		((a) > 0.0 ? (1.0) : (-1.0))
#define SIGN1(a, b) ((b) > 0.0 ? ABS(a) : -ABS(a))
#define SIGN2(a, b)	((b) >= 0.0 ? fabs(a) : -fabs(a))
#define PYTHAG(a, b) SQRT((SQR(a) + SQR(b)))
#define EQ(X, Y, EPS)	(ABS((X) - (Y)) < EPS)
#define EQ_ZERO(X, EPS) (ABS(X) < EPS)
#define ARR_ZERO(A, N) memset((A), 0, sizeof(A[0]) * (N))
#define ARR_COPY(D, S, N) memmove((D), (S), sizeof(S[0]) * (N))
#define ARR_COPY_2D(D, S, M, N) \
	for (int i = 0; i < (M); ARR_COPY(D[i], S[i], N), ++i);
#define ARR_ALLOC(D, TYPE, N) D = new TYPE [(N)]
#define ARR_ALLOC_2D(D, TYPE, M, N) \
	D = new TYPE *[(M)]; \
	for (int i = 0; i < (M); D[i] = new TYPE [(N)], ++i);
#define ARR_FREE(D) \
	delete [] D; \
	D = NULL
#define ARR_FREE_2D(D, M) \
	for (int i = 0; i < (M); delete [] D[i], ++i); \
	delete [] D; \
	D = NULL
#define MTYPE_EPS	1.0e-15

// Ŭ���� ����
class GVec3;
class GVec4;
class GVec;
class GPos3;
class GMatrix;
class GQuater;
class GTransf;
class GLine;
class GPlane;

/*!
*	\class GVec2
*	\brief 2���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GVec2
{
	// ������ �Լ� �� Ŭ����
	friend GVec2 operator +(const GVec2 &lhs, const GVec2 &rhs);
	friend GVec2 operator -(const GVec2 &lhs, const GVec2 &rhs);
	friend GVec2 operator *(const GVec2 &lhs, const double &s);
	friend GVec2 operator *(const double &s, const GVec2 &rhs);
	friend double operator *(const GVec2 &lhs, const GVec2 &rhs);
	friend GVec2 operator /(const GVec2 &lhs, const double &s);
	friend bool operator ==(const GVec2 &lhs, const GVec2 &rhs);
	friend bool operator !=(const GVec2 &lhs, const GVec2 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVec2 &v);
	friend std::istream &operator >>(std::istream &is, GVec2 &v);
	friend GVec2 proj(const GVec2 &v, const GVec2 &w);
	friend double dist(const GVec2 &v, const GVec2 &w);
	friend double norm(const GVec2 &v);
	friend double angle(const GVec2 &v, const GVec2 &w, bool radian = false);
	friend GVec cast_vec(const GVec2 &v);
	friend GVec3 cast_vec3(const GVec2 &v);

public:
	// ������ �� �Ҹ���
	GVec2(double x = 0.0, double y = 0.0);
	GVec2(const GVec2 &cpy);
	~GVec2();

	// ���� �� ���մ��� ������
	GVec2 &operator =(const GVec2 &rhs);
	GVec2 &operator +=(const GVec2 &rhs);
	GVec2 &operator -=(const GVec2 &rhs);
	GVec2 &operator *=(const double &s);
	GVec2 &operator /=(const double &s);

	// ���� ������
	GVec2 operator +() const;
	GVec2 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVec2 &Set(const double &x, const double &y);
	GVec2 &Normalize();

	// ���� �ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 2���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[2];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVec3
*	\brief 3���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GVec3
{
	// ������ �Լ� �� Ŭ����
	friend GVec3 operator +(const GVec3 &lhs, const GVec3 &rhs);
	friend GPos3 operator +(const GVec3 &lhs, const GPos3 &rhs);
	friend GVec3 operator -(const GVec3 &lhs, const GVec3 &rhs);
	friend GVec3 operator *(const GVec3 &lhs, const double &s);
	friend GVec3 operator *(const double &s, const GVec3 &rhs);
	friend double operator *(const GVec3 &lhs, const GVec3 &rhs);
	friend GVec3 operator /(const GVec3 &lhs, const double &s);
	friend GVec3 operator ^(const GVec3 &lhs, const GVec3 &rhs);
	friend bool operator ==(const GVec3 &lhs, const GVec3 &rhs);
	friend bool operator !=(const GVec3 &lhs, const GVec3 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVec3 &v);
	friend std::istream &operator >>(std::istream &is, GVec3 &v);
	friend GVec3 proj(const GVec3 &v, const GVec3 &w);
	friend GVec3 ortho(const GVec3 &v);
	friend double dist(const GVec3 &v, const GVec3 &w);
	friend double det(const GVec3 &u, const GVec3 &v, const GVec3 &w);
	friend double norm(const GVec3 &v);
	friend double angle(const GVec3 &v, const GVec3 &w, bool radian = false);
	friend double angle(const GVec3 &v, const GVec3 &w, const GVec3 &up, bool radian = false);
	friend GPos3 cast_pt3(const GVec3 &v);
	friend GVec cast_vec(const GVec3 &v);
	friend GVec4 cast_vec4(const GVec3 &v, double w = 0.0);

public:
	// ������ �� �Ҹ���
	GVec3(double x = 0.0, double y = 0.0, double z = 0.0);
	GVec3(const GVec3 &cpy);
	~GVec3();

	// ���� �� ���մ��� ������
	GVec3 &operator =(const GVec3 &rhs);
	GVec3 &operator +=(const GVec3 &rhs);
	GVec3 &operator -=(const GVec3 &rhs);
	GVec3 &operator *=(const double &s);
	GVec3 &operator /=(const double &s);
	GVec3 &operator ^=(const GVec3 &rhs);

	// ���� ������
	GVec3 operator +() const;
	GVec3 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVec3 &Set(const double &x, const double &y, const double &z);
	GVec3 &Normalize();

	// ���� �ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 3���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[3];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVec4
*	\brief 4���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GVec4
{
	// ������ �Լ� �� Ŭ����
	friend GVec4 operator +(const GVec4 &lhs, const GVec4 &rhs);
	friend GVec4 operator -(const GVec4 &lhs, const GVec4 &rhs);
	friend GVec4 operator *(const GVec4 &lhs, const double &s);
	friend GVec4 operator *(const double &s, const GVec4 &rhs);
	friend double operator *(const GVec4 &lhs, const GVec4 &rhs);
	friend GVec4 operator /(const GVec4 &lhs, const double &s);
	friend bool operator ==(const GVec4 &lhs, const GVec4 &rhs);
	friend bool operator !=(const GVec4 &lhs, const GVec4 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVec4 &v);
	friend std::istream &operator >>(std::istream &is, GVec4 &v);
	friend double norm(const GVec4 &v);
	friend double dist(const GVec4 &v, const GVec4 &w);
	friend GVec cast_vec(const GVec4 &v);
	friend GVec3 cast_vec3(const GVec4 &v);
	friend GPos3 cast_pt3(const GVec4 &v);
	friend GVec4 homogenize(const GVec4 &v);
	friend GVec4 cartesianize(const GVec4 &v);
	friend GVec4 rationalize(const GVec4 &v);

public:
	// ������ �� �Ҹ���
	GVec4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 0.0);
	GVec4(const GVec4 &cpy);
	~GVec4();

	// ���� �� ���մ��� ������
	GVec4 &operator =(const GVec4 &rhs);
	GVec4 &operator +=(const GVec4 &rhs);
	GVec4 &operator -=(const GVec4 &rhs);
	GVec4 &operator *=(const double &s);
	GVec4 &operator /=(const double &s);

	// ���� ������
	GVec4 operator +() const;
	GVec4 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVec4 &Set(double x, double y, double z, double w);
	GVec4 &Normalize();

	// �����ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 4���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[4];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVec
*	\brief \a n���� ���� �ڷ����� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GVec
{
	// ������ �Լ� �� Ŭ����
	friend GVec operator +(const GVec &lhs, const GVec &rhs);
	friend GVec operator -(const GVec &lhs, const GVec &rhs);
	friend double operator *(const GVec &lhs, const GVec &rsh);
	friend GVec operator *(const double &s, const GVec &rhs);
	friend GVec operator *(const GVec &lhs, const double &s);
	friend GVec operator *(const GMatrix &lhs, const GVec &rhs);
	friend GMatrix operator *(const GVec &lhs, const GMatrix &rhs);
	friend GVec operator /(const GVec &lhs, const double &s);
	friend bool operator >(const GVec &lhs, const GVec &rhs);
	friend bool operator >=(const GVec &lhs, const GVec &rhs);
	friend bool operator <(const GVec &lhs, const GVec &rhs);
	friend bool operator <=(const GVec &lhs, const GVec &rhs);
	friend bool operator ==(const GVec &lhs, const GVec &rhs);
	friend bool operator !=(const GVec &lhs, const GVec &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVec &v);
	friend std::istream &operator >>(std::istream &is, GVec &v);
	friend double norm(const GVec &v);
	friend double dist(const GVec &v, const GVec &w);
	friend int dcmp_lu(GMatrix &A, GVec &Idx, double &d);
	friend void dcmp_sv(const GMatrix &A, GMatrix &U, GVec &S, GMatrix &V);
	friend void lin_sys_lu(const GMatrix &A, GVec &x, const GVec &b, const GVec &Idx);
	friend double lin_sys_sv(const GMatrix &A, GVec &x, const GVec &b);
	friend double lin_sys_sv(const GMatrix &U, const GVec &S, const GMatrix &V, GVec &x, const GVec &b);
	friend int lin_sys_cg(const GMatrix &A, GVec &x, const GVec &b, int iter);
	friend int lin_sys_qr(const GMatrix &A, GVec &x, const GVec &b);
	friend int eigen_sys(const GMatrix &A, GVec &D, GMatrix &V);
	friend void pca(GMatrix &Data, GMatrix &PC, GVec &SigVal, GVec &Mean);
	friend double *cast_arr(const GVec &v);
	friend GPos3 cast_pt3(const GVec &v);
	friend GVec3 cast_vec3(const GVec &v);
	friend GQuater cast_quat(const GVec &v);
	friend class GMatrix;

public:
	// ������ �� �Ҹ���
	GVec(int dim = 3);
	GVec(int dim, double x, ...);
	GVec(int dim, int x, ...);
	GVec(const GVec &cpy);
	~GVec();

	// ���Կ����� �� ���մ��� ������
	GVec &operator =(const GVec &rhs);
	GVec &operator +=(const GVec &rhs);
	GVec &operator -=(const GVec &rhs);
	GVec &operator *=(const double &s);
	GVec	&operator /=(const double &s);

	// ���� ������
	GVec operator +() const;
	GVec operator -() const;

	// �ε��� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVec	&Set(double x, ...);
	GVec	&Set(int x, ...);
	GVec &Set(double *pVal);
	GVec	&Normalize();
	GVec &SetZeros();
	int GetDim() const;

	// �����Լ�
	static void	SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief ������ ���� */
	int N;

	/*! \brief ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double *V;

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GPos3
*	\brief 3���� ����Ʈ �ڷ����� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GPos3
{
	// ������ �Լ� �� Ŭ����
	friend GVec3 operator -(const GPos3 &lhs, const GPos3 &rhs);
	friend GPos3 operator -(const GPos3 &lhs, const GVec3 &rhs);
	friend GPos3 operator +(const GPos3 &lhs, const GVec3 &rhs);
	friend GPos3 operator +(const GVec3 &lhs, const GPos3 &rhs);
	friend bool operator ==(const GPos3 &lhs, const GPos3 &rhs);
	friend bool operator !=(const GPos3 &lhs, const GPos3 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GPos3 &p);
	friend std::istream &operator >>(std::istream &is, GPos3 &p);
	friend double norm(const GPos3 &p);
	friend double dist(const GPos3 &p, const GPos3 &q);
	friend double dist_sq(const GPos3 &p, const GPos3 &q);
	friend GPos3 affine_sum(int N, ...);
	friend GPos3 affine_sum(GPos3 *Points, double *Weights, const int Size);
	friend GVec3 cast_vec3(const GPos3 &p);
	friend GVec4 cast_vec4(const GPos3 &p, double w = 0.0);
	friend GVec cast_vec(const GPos3 &p);
	friend GPos3 round_off(const GPos3 &p, int place);

public:
	// ������ �� �Ҹ���
	GPos3(double x = 0.0, double y = 0.0, double z = 0.0);
	GPos3(const GPos3 &cpy);
	~GPos3();

	// ���� ������
	GPos3 &operator =(const GPos3 &rhs);
	GPos3 &operator +=(const GVec3 &rhs);

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GPos3 &Set(const double &x, const double &y, const double &z);

	// ���� ����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	/*! \brief 3���� ������ ���Ҹ� �����ϴ� �Ǽ��迭 */
	double V[3];
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GMatrix
*	\brief \a m �� \a n ���� ũ�⸦ ���� ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GMatrix
{
	// ������ �Լ� �� Ŭ����
	friend GMatrix operator +(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator -(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator *(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator /(const GMatrix &lhs, const double &s);
	friend bool operator ==(const GMatrix &lhs, const GMatrix &rhs);
	friend bool operator !=(const GMatrix &lhs, const GMatrix &rhs);
	friend GVec operator *(const GMatrix &lhs, const GVec &rhs);
	friend GMatrix operator *(const GVec &lhs, const GMatrix &rhs);
	friend GMatrix operator *(const GMatrix &lhs, const double &s);
	friend GMatrix operator *(const double &s, const GMatrix &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GMatrix &m);
	friend GMatrix tr(const GMatrix &m);
	friend GMatrix ref(const GMatrix &m);
	friend GMatrix rref(const GMatrix &m);
	friend GMatrix basis_null(const GMatrix &m);
	friend double *cast_arr(const GMatrix &m);
	friend int rank(const GMatrix &m);
	friend int nullity(const GMatrix &m);
	friend double det(const GMatrix &A);
	friend GMatrix inv(const GMatrix &A);
	friend int dcmp_lu(const GMatrix &A, GMatrix &L, GMatrix &U);
	friend int dcmp_lu(GMatrix &A, GVec &Idx, double &d);
	friend int dcmp_qr(const GMatrix &A, GMatrix &Q, GMatrix &R);
	friend void dcmp_sv(const GMatrix &A, GMatrix &U, GVec &S, GMatrix &V);
	friend void lin_sys_lu(const GMatrix &A, GVec &x, const GVec &b, const GVec &Idx);
	friend double lin_sys_sv(const GMatrix &A, GVec &x, const GVec &b);
	friend double lin_sys_sv(const GMatrix &U, const GVec &S, const GMatrix &V, GVec &x, const GVec &b);
	friend int lin_sys_cg(const GMatrix &A, GVec &x, const GVec &b, int iter);
	friend int lin_sys_qr(const GMatrix &A, GVec &x, const GVec &b);
	friend int eigen_sys(const GMatrix &A, GVec &D, GMatrix &V);
	friend void pca(GMatrix &Data, GMatrix &PC, GVec &SigVal, GVec &Mean);

public:
	// ������ �� �Ҹ���
	GMatrix(int row = 4, int col = 4, double *elem = NULL);
	GMatrix(const GMatrix &cpy);
	~GMatrix();

	// ���� ������ �� ���մ��� ������
	GMatrix &operator =(const GMatrix &rhs);
	GMatrix &operator +=(const GMatrix &rhs);
	GMatrix &operator -=(const GMatrix &rhs);
	GMatrix &operator *=(const GMatrix &rhs);
	GMatrix &operator *=(const double &s);
	GMatrix &operator /=(const double &s);

	// ���� ������
	GMatrix operator +() const;
	GMatrix operator -() const;

	// ÷�� ������
	double *operator [](const int idx);
	const double *operator [](const int idx) const;

	// ����Լ�
	GMatrix &SetTranspose();
	GMatrix &SetIdentity();
	GMatrix &SetZeros();
	GMatrix &SetRowVec(const int idx, const GVec &v);
	GMatrix &SetColVec(const int idx, const GVec &v);
	GMatrix &ExchangeRows(const int idx0, const int idx1);
	GMatrix &ExchangeCols(const int idx0, const int idx1);
	int GetRowNum() const;
	int GetColNum() const;
	GVec GetRowVec(const int idx) const;
	GVec GetColVec(const int idx) const;
	bool IsSquare() const;

	// �����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	/*! \brief ����� ���� ���� */
	int r;
	/*! \brief ����� ���� ���� */
	int c;
	/*! \brief ����� ���Ҹ� �����ϴ� 1���� �Ǽ� �迭 */
	double *M;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GQuater
*	\brief ���ʹϿ� (q = W + iX + jY + kZ)�� ��Ÿ���� Ŭ����.
*	\warning �������ʹϿ��� �ƴѰ�쵵 ����.
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
enum TypeEuler
{
	EULER_XYZ = 0,
	EULER_ZYX = 1,
};

enum TypeArcBall
{
	ARCBALL_FREE = 0,
	ARCBALL_X,
	ARCBALL_Y,
	ARCBALL_Z
};

class GQuater
{
	// ����Ʈ �Լ� �� Ŭ����
	friend GQuater operator +(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator -(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator *(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator /(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator /(const GQuater &lhs, const double s);
	friend GPos3 operator *(const GQuater &lhs, const GPos3 &rhs);
	friend GVec3 operator *(const GQuater &lhs, const GVec3 &rhs);
	friend bool operator ==(const GQuater &lhs, const GQuater &rhs);
	friend bool operator !=(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator *(const GQuater &lhs, const double &s);
	friend GQuater operator *(const double &s, const GQuater &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GQuater &q);
	friend std::istream &operator >>(std::istream &is, GQuater &q);
	friend double norm(const GQuater &q);
	friend GQuater inv(const GQuater &q);
	friend GQuater exp(const GVec3 &v);
	friend GVec3 log(const GQuater &q);
	friend double quat_dot(const GQuater &q1, const GQuater &q2);
	friend GQuater slerp(GQuater q1, GQuater q2, double t);
	friend GMatrix cast_mat(const GQuater &q, bool isGL = true);
	friend GVec cast_vec(const GQuater &q);
	friend GVec4 cast_vec4(const GQuater &q);
	friend GQuater cast_qt(const GVec4 &v);

public:
	// ������ �� �Ҹ���
	GQuater(double w = 1.0, double x = 0.0, double y = 0.0, double z = 0.0);
	GQuater(const GQuater &cpy);
	GQuater(const double *q, const bool invOrder = false);
	GQuater(GVec3 axis, double theta, bool radian = false);
	GQuater(double theta1, double theta2, double theta3, TypeEuler eulerType = EULER_XYZ);
	GQuater(GVec3 x_axis, GVec3 y_axis);
	~GQuater();

	// ���� ������
	GQuater &operator =(const GQuater &rhs);

	// ���մ��� ������
	GQuater &operator +=(const GQuater &rhs);
	GQuater &operator -=(const GQuater &rhs);
	GQuater &operator *=(const GQuater &rhs);
	GQuater &operator /=(const GQuater &rhs);
	GQuater &operator *=(const double s);
	GQuater &operator /=(const double s);

	// ���� ������
	GQuater operator +() const;
	GQuater operator -() const;

	// Set ����Լ�
	GQuater &Set(const double w, const double x, const double y, const double z);
	GQuater &Set(double *q, bool invOrder = false);
	GQuater &SetIdentity();
	GQuater &SetInverse();
	GQuater &SetConjugate();
	GQuater &SetNegate();
	GQuater &SetFromArcBall(GVec3 p, GVec3 q, TypeArcBall arcType = ARCBALL_FREE);
	GQuater &SetFromAngleAxis(const double theta, GVec3 axis, bool radian = false);
	GQuater &SetFromEulerAngle(double theta1, double theta2, double theta3, TypeEuler eulerType = EULER_XYZ);
	GQuater &SetFromFrameXY(const GVec3 &x_axis, const GVec3 &y_axis);
	GQuater &SetFromFrameYZ(const GVec3 &y_axis, const GVec3 &z_axis);
	GQuater &SetFromFrameZX(const GVec3 &z_axis, const GVec3 &x_axis);
	GQuater &SetFromMatrix(double *mat, bool isGL);
	GQuater &Normalize();

	// Get ����Լ�
	void	GetAngleAxis(GVec3 &axis, double &angle, bool radian = false) const;
	void	GetEulerAngle(double &theta1, double &theta2, double &theta3, bool radian = false, TypeEuler eulerType = EULER_XYZ);
	double	GetW() const;
	double	GetX() const;
	double	GetY() const;
	double	GetZ() const;

	// ����Լ�
	bool IsUnitQuater() const;
	bool IsIdentity() const;

	// ���� ����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief ���ʹϿ��� �Ǽ��� */
	double W;
	/*! \brief ���ʹϿ��� ����� */
	double X, Y, Z;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GLine
*	\brief 3���� �������� ������ ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GLine
{
	// ������ �Լ� �� Ŭ����
	friend bool operator ==(const GLine &lhs, const GLine &rhs);
	friend bool operator !=(const GLine &lhs, const GLine &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GLine &l);
	friend double dist(const GLine &l, const GPos3 &q);
	friend double dist(const GLine &l0, const GLine &l1, double &t1, double &t2);
	friend bool intersect_line_line(GPos3 &p, const GLine &l0, const GLine &l1, double &t0, double &t1);
	friend bool intersect_line_line2(GPos3 &p, GLine &l0, GLine &l1);
	friend bool intersect_line_plane(GPos3 &p, const GLine &l, const GPlane &pi);
	friend bool intersect_line_triangle(const GPos3 &p1, const GPos3 &p2, const GPos3 &p3, const GLine &l, double &t, GPos3 &q, double &alpha, double &beta, double &gamma, const bool bCull = true);
	friend bool intersect_line_triangle(const GPos3 &p1, const GPos3 &p2, const GPos3 &p3, const GLine &l, GPos3 &ispt, const bool bCull = true);
	friend bool intersect_planes(GLine &l, const GPlane &pi1, const GPlane &pi2);
	friend GLine operator *(const GTransf &X, const GLine &l);

public:
	// ������ �� �Ҹ���
	GLine(const GPos3 &_p = GPos3(0, 0, 0), const GVec3 &_v = GVec3(0, 0, 0));
	GLine(const GPos3 &_p, const GPos3 &_q);
	GLine(const GLine &cpy);
	~GLine();

	// ���� ������
	GLine &operator =(const GLine &rhs);

	// ����Լ�
	GLine &SetPt(const GPos3 &_p);
	GLine &SetDir(const GVec3 &_v);
	GPos3 GetPt() const;
	GVec3 GetDir() const;
	bool IsOnLine(const GPos3 &q) const;
	GPos3 Eval(const double t) const;

	// ��������Լ�
	static void SetPrecision(double err);
	static double GetPrecision();

public:
	// ������ ���.
	/*! \brief �������� �� �� */
	GPos3 p;
	/*! \brief ������ ���� ���� l(t) = p + v * t */
	GVec3 v;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GPlane
*	\brief ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GPlane
{
	// ������ �Լ� �� Ŭ����
	friend bool operator ==(const GPlane &lhs, const GPlane &rhs);
	friend bool operator !=(const GPlane &lhs, const GPlane &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GPlane &pi);
	friend GPlane operator *(const GTransf &X, const GPlane &pi);
	friend double dist(const GPlane &pi, const GPos3 &p);
	friend GPos3 proj(const GPos3 &p, const GPlane &pi);
	friend bool intersect_line_plane(GPos3 &p, const GLine &l, const GPlane &pi);
	friend bool intersect_planes(GLine &l, const GPlane &pi1, const GPlane &pi2);
	friend bool intersect_planes(GPos3 &p, const GPlane &pi1, const GPlane &pi2, const GPlane &pi3);

public:
	// ������ �� �Ҹ���
	GPlane(const GVec3 &_n, const GPos3 &_p);
	GPlane(const GPos3 &p1, const GPos3 &p2, const GPos3 &p3);
	GPlane(const GVec3 &_n, const double &_d);
	GPlane(const double &a, const double &b, const double &c, const double &d);
	GPlane(const GPlane &cpy);
	~GPlane();

	// ���� ������
	GPlane &operator =(const GPlane &rhs);

	// ����Լ�
	GVec3 GetNormal() const;
	bool IsOnPlane(const GPos3 &p) const;
	bool IsAbovePlane(const GPos3 &p) const;
	bool IsBelowPlane(const GPos3 &p) const;

	// ���� ����Լ�
	static void SetPrecision(const double err);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief ����� ������(ax + by + cz + d = 0)���� ���� ���� n=(a, b, c) */
	GVec3 n;
	/*! \brief ����� ������(ax + by + cz + d = 0)���� ��� d(= -n * p) */
	double d;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GTransf
*	\brief ��ü��ȯ(rigid-body transformation)�� ��Ÿ���� Ŭ����.
*
*	\author ������(shyun@dongguk.edu)
*	\date 2001�� 1�� 1��.
*/
class GTransf
{
	// ������ �Լ� �� Ŭ����
	friend GTransf operator *(const GTransf &lhs, const GTransf &rhs);
	friend bool operator ==(const GTransf &lhs, const GTransf &rhs);
	friend bool operator !=(const GTransf &lhs, const GTransf &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GTransf &X);
	friend GTransf operator *(const GTransf &lhs, const double &s);
	friend GTransf operator *(const double &s, const GTransf &rhs);
	friend GPos3 operator *(const GTransf &X, const GPos3 &p);
	friend GLine operator *(const GTransf &X, const GLine &l);
	friend GPlane operator *(const GTransf &X, const GPlane &pi);
	friend GVec3 operator *(const GTransf &X, const GVec3 &v);
	friend GTransf inv(const GTransf &X);
	friend GVec3 part_trans(const GTransf &X);
	friend GQuater part_rot(const GTransf &X);
	friend GMatrix cast_mat(const GTransf &X, bool isGL = true);
	friend double norm(const GTransf &X);
	friend double dist(const GTransf &X1, const GTransf &X2);
	friend GTransf ad(const GTransf &X1, const GTransf &dX, const GTransf &X2);
	friend GTransf lerp(GTransf X1, GTransf X2, const double t);

public:
	// ������ �� �Ҹ���
	GTransf(const GVec3 &_t = GVec3(0.0, 0.0, 0.0), const GQuater &_q = GQuater(1.0, 0.0, 0.0, 0.0));
	GTransf(double *mat, bool isGL);
	GTransf(const GTransf &cpy);
	~GTransf();

	// ���� ������ �� ���մ��� ������
	GTransf &operator =(const GTransf &rhs);
	GTransf &operator *=(const GTransf &rhs);
	GTransf &operator *=(const double &s);

	// ����Լ�
	void SetNegate();
	void SetTrans(const GVec3 &_t);
	void SetRot(const GQuater &_q);

public:
	// ������ ���
	/*! \brief 3���� �̵���ȯ */
	GVec3 t;
	/*! \brief 3���� ȸ����ȯ */
	GQuater q;
};

#endif // _GMATH_H