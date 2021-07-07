#pragma once

#include <vector>
#include "gmath.h"
#include ".\usr\include\GL\freeglut.h"

/*!
*	\brief ������ ��� ǥ���ϴ� Ŭ����
*/
class GBzrCrv
{
public:
	/*! \brief ��� ���ǿ� [a, b]*/
	GVec2	m_Domain;

	/*! \brief ������ �迭 (��� ���� = ������ �� - 1) */
	std::vector<GVec3>	m_Points;

public:
	// ������ �� �Ҹ���
	GBzrCrv() 
	{ 
		m_Domain = GVec2(0.0, 1.0); 
	}
	GBzrCrv(GVec2 Domain, std::vector<GVec3> Points) 
	{ 
		m_Domain = Domain;
		m_Points = Points; 
	}
	~GBzrCrv() {};

public:
	/*!
	*	\brief � ���� �� C(u)�� ���� ���� C'(u)�� ����Ѵ�.
	*
	*	\param	u[in]	��� �Ķ���� (a <= u <= b)
	*	\param	p[out]	� ���� �� C(u)�� ����ȴ�.
	*	\param	v[out]	���� ���� C'(u)�� ����ȴ�.
	*/
	void Eval(double u, GVec3 &p, GVec3 &v)
	{
		std::vector<GVec3> Points = m_Points;
		double t = (u - m_Domain[0]) / (m_Domain[1] - m_Domain[0]);
				
		// de_Casteljau �˰����� �����Ѵ�.
		int NumPts = (int)Points.size();
		for (int i = 1; i < NumPts; ++i) // i = 1, ... (NumPt - 1)
		{
			for (int j = 0; j < NumPts - i; ++j)
				Points[j] = (1.0 - t) * Points[j] + t * Points[j + 1];

			if (i == NumPts - 2) // ������ ���ܰ���... ���� �̺��� ���Ѵ�.
				v = (NumPts - 1) * (Points[1] - Points[0]) / (m_Domain[1] - m_Domain[0]);
		}
		p = Points[0];
	}

	/*!
	*	\brief ��� ������ �Ѵ�.
	*
	*	\param	NumOfPts[in]	� ���� ���� ��
	*/
	void Render(int NumOfPts)
	{
		int NumPts = (int)m_Points.size();

		// �������� ������ �Ѵ�.
		glColor3d(1.0, 0.0, 0.0);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		{
			for (int i = 0; i < NumPts; ++i)
				glVertex3d(m_Points[i][0], m_Points[i][1], m_Points[i][2]);
		}
		glEnd();

		// ����ٰ����� ������ �Ѵ�.
		glColor3d(0.0, 1.0, 0.0);
		glLineWidth(2.0);
		glBegin(GL_LINE_STRIP);
		{
			for (int i = 0; i < NumPts; ++i)
				glVertex3d(m_Points[i][0], m_Points[i][1], m_Points[i][2]);
		}
		glEnd();

		// ��� ������ �Ѵ�.
		glColor3f(0.0, 0.0, 1.0);
		glLineWidth(3.0);
		glBegin(GL_LINE_STRIP);
		{
			GVec3 p, v;
			for (int i = 0; i < NumOfPts; ++i)  // i = 0, ... NumOfPts - 1
			{
				double u = m_Domain[0] + (m_Domain[1] - m_Domain[0]) * (double)i / (NumOfPts - 1); // a <= u <= b
				Eval(u, p, v);
				glVertex3d(p[0], p[1], p[2]);
			}
		}
		glEnd();
		glLineWidth(1.0);
		glPointSize(1.0);
	}
};

