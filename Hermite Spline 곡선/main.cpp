#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "gmath.h"
#include "bzrcrv.h"
#include ".\usr\include\GL\freeglut.h"

using namespace std;
int Width = 800, Height = 800;
int ManipulateMode = 0;

// ������ȯ�� ���� ����
int StartPt[2];
float Angle = 0.0;
GVec3 Axis;
float Zoom = -50.0;
float Pan[3] = { 0.0, 0.0, 0.0 };
float RotMat[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
double gTime = 0.0;

// ���� �������� �ʱⰪ
std::vector<GVec3> gPoints;
std::vector<GBzrCrv> gCrvList; // ������ ��� ����Ʈ

// Callback �Լ�
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);
void Timer(int id);

// ����� �����Լ�
void GetSphereCoord(int x, int y, GVec3& p);
void Render();
void RenderFloor();
void SetupViewVolume();
void SetupViewTransform();
void get_interp_crv_hermite(std::vector<GVec3>& Points, std::vector<GBzrCrv>& CrvList);

int main(int argc, char** argv)
{
	// GLUT �ʱ�ȭ
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// ������ �ʱ�ȭ �� ����
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Bezier Curves");

	// ���� ���� ����
	gPoints.push_back(GVec3(0.0, 5.0, 0.0));
	gPoints.push_back(GVec3(3.5, 7.0, 0.0));
	gPoints.push_back(GVec3(7.0, 5.0, 0.0));
	gPoints.push_back(GVec3(5.0, 0.0, 0.0));
	gPoints.push_back(GVec3(0.0, -5.0, 0.0));
	gPoints.push_back(GVec3(0.0, 5.0, 0.0));

	// ������ ���� ��� ���Ѵ�.
	get_interp_crv_hermite(gPoints, gCrvList);

	// Callback �Լ� ���
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutKeyboardFunc(Keyboard);
	//glutTimerFunc(30, Timer, 0);

	// �޽��� ��Ǫ ����
	glutMainLoop();
	return 0;
}

void Render()
{
	// Į�� ���ۿ� ���� ���۸� �ʱ�ȭ �Ѵ�.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���� ������ �����ϰ�, ���� ��ȯ�� �����Ѵ�.
	SetupViewVolume();
	SetupViewTransform();

	// �� ����Ʈ�� �������Ѵ�.
	glMatrixMode(GL_MODELVIEW);
	RenderFloor();

	// �������� ������ �Ѵ�.
	glColor3f(1.0, 0.0, 0.0); // ������.
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < gPoints.size(); ++i)
		glVertex3d(gPoints[i][0], gPoints[i][1], gPoints[i][2]);
	glEnd();
	glPointSize(1.0);

	for (int i = 0; i < gCrvList.size(); ++i)
		gCrvList[i].Render(50);

	// Į�� ���۸� ��ȯ�Ѵ�.
	glutSwapBuffers();
}

/*!
*	\brief	������ �����ϴ� 3�� ������ ��� ����Ʈ�� �����Ѵ�.
*
*	\param	Points[in]		������ ������ ����
*	\param	CrvList[out]	������ ������ ��� ����ȴ�.
*/
void get_interp_crv_hermite(std::vector<GVec3>& Points, std::vector<GBzrCrv>& CrvList)
{
	// 1. ���� �Ķ���͸� �����Ѵ�. -> chord length �Ķ����
	int NumPoints = (int)Points.size();
	vector<double> Params(NumPoints, 0.0); // �� ������ŭ �Ķ���͸� ������ ���� ����
	for (int i = 1; i < NumPoints; ++i)
		Params[i] = Params[i - 1] + dist(Points[i], Points[i - 1]);

	// 2. �����͸� �����Ѵ�.
	GVec3 zero(0.0, 0.0, 0.0); // ������
	vector<GVec3> TanVec(NumPoints, zero); // �ʱⰪ �������� �������� �迭 ����

	//���� ���� ���ϱ�
	for (int i = 1; i < NumPoints-1 ; ++i)
		TanVec[i] = (Points[i + 1] - Points[i - 1]) / (Params[i + 1] - Params[i - 1]);

	// ������ ��� �׸��� ���� �������� ���ϱ�
	if (Points[0] == Points[NumPoints - 1])
		TanVec[0] = TanVec[NumPoints - 1] = (Points[1] - Points[NumPoints - 2]) / dist(Points[1], Points[NumPoints - 2]);

	// 3. �� ������ ������ ��� �����Ͽ� ����Ʈ�� �߰��Ѵ�.
	vector<GVec3> CtrPts(4, GVec3()); // 3�� hermite ��� ����� ���� �������� ������ vector
	for (int i = 0; i < NumPoints-1; ++i) {
		// ��� ���ǿ�
	    GVec2 Domain = GVec2(Params[i], Params[i + 1]);

		// ������ ���ϱ�			
		CtrPts[0] = Points[i]; // 1�� ������
		CtrPts[1] = Points[i] + ((Domain[1] - Domain[0]) / 3) * TanVec[i]; // 2�� ������
		CtrPts[3] = Points[i + 1]; // 4�� ������
		CtrPts[2] = Points[i + 1] - ((Domain[1] - Domain[0]) / 3) * TanVec[i + 1]; // 3�� ������
		
		// � �����
		CrvList.push_back(GBzrCrv(Domain, CtrPts));
	}
}

void Reshape(int w, int h)
{
	// ����Ʈ ��ȯ�� �����Ѵ�.
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void Keyboard(unsigned char key, int x, int y)
{
	// ESC Ű�� ������ �޽� ����Ʈ�� �޸𸮿��� �����ϰ� �����Ѵ�.
	if (key == 27)
		exit(0);
}

void SetupViewTransform()
{
	// �𵨺� ��� ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// �̵�-ȸ�� ��ȯ ����
	glTranslatef(0.0, 0.0, Zoom);
	glRotatef(Angle, Axis[0], Axis[1], Axis[2]);

	// ������ ����� ȸ�� ��ȯ ��, �̵� ��ȯ ����
	glMultMatrixf(RotMat);
	glTranslatef(Pan[0], Pan[1], Pan[2]);

	// ȸ�� ��ȯ�� ����� ����
	glGetFloatv(GL_MODELVIEW_MATRIX, RotMat);
	RotMat[12] = RotMat[13] = RotMat[14] = 0.0;
}

void SetupViewVolume()
{
	// ���� ��� ���� �� ���� ���� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)Width / (double)Height, 0.01, 10000.0);
}

void RenderFloor()
{
	// �ٴ� ����� �������Ѵ�.
	//glDisable(GL_LIGHTING);
	glColor3d(0.7, 0.7, 0.7);
	for (int i = -50; i <= 50; ++i)
	{
		if (i == 0)
			glLineWidth(2.0);
		else
			glLineWidth(1.0);
		glBegin(GL_LINES);
		glVertex3i(i, -50, -0.001);
		glVertex3f(i, 50, -0.001);
		glVertex3i(-50, i, -0.001);
		glVertex3i(50, i, -0.001);
		glEnd();
	}
	//glEnable(GL_LIGHTING);
}

void Mouse(int button, int state, int x, int y)
{
	// ���콺 ��ư�� ���� ���,
	if (state == GLUT_DOWN)
	{
		StartPt[0] = x;
		StartPt[1] = y;

		if (button == GLUT_LEFT_BUTTON)
			ManipulateMode = 1;		// ȸ��
		else if (button == GLUT_RIGHT_BUTTON)
			ManipulateMode = 2;		// ����/�ƿ�
		else
			ManipulateMode = 3;		// �̵�
	}

	// ���콺 ��ư�� �� ���,
	if (state == GLUT_UP)
	{
		// �� �ʱ�ȭ
		ManipulateMode = 0;
		StartPt[0] = StartPt[1] = 0;
		Angle = 0.0;
	}
}

void MouseMove(int x, int y)
{
	// ȸ�� �����,
	if (ManipulateMode == 1)
	{
		GVec3 p, q;
		GetSphereCoord(StartPt[0], StartPt[1], p);
		GetSphereCoord(x, y, q);

		Axis = p ^ q;
		if (norm(Axis) < 0.000001)
		{
			Axis.Set(0.0, 0.0, 0.0);
			Angle = 0.0;
		}
		else
		{
			Axis.Normalize();
			//std::cout << Axis << std::endl;
			Angle = p * q;
			Angle = acos(Angle) * 180.0f / 3.141592f;
		}
	}
	else  if (ManipulateMode == 2) // Zoom �����,
	{
		Zoom += (StartPt[1] - y) * 0.1;
		if (Zoom > -0.001)
			Zoom = -0.001;
	}
	else	// �̵� �����,
	{
		float dx = (x - StartPt[0]) * 0.1;
		float dy = (StartPt[1] - y) * 0.1;
		Pan[0] += RotMat[0] * dx + RotMat[1] * dy;
		Pan[1] += RotMat[4] * dx + RotMat[5] * dy;
		Pan[2] += RotMat[8] * dx + RotMat[9] * dy;
	}

	StartPt[0] = x;
	StartPt[1] = y;
	glutPostRedisplay();
}

void GetSphereCoord(int x, int y, GVec3& p)
{
	p[0] = (2.0 * x - Width) / Width;
	p[1] = (-2.0 * y + Height) / Height;
	p[2] = 0.0;

	double r = p * p;
	if (r >= 1.0)
		p.Normalize();
	else
		p[2] = sqrt(1.0 - r);
}

