#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "gmath.h"
#include "bzrcrv.h"
#include ".\usr\include\GL\freeglut.h"

using namespace std;
int Width = 800, Height = 800;
int ManipulateMode = 0;
int Keynum = 6;

// ������ȯ�� ���� ����
int StartPt[2];
float Angle = 0.0;
GVec3 Axis;
float Zoom = -50.0;
float Pan[3] = { 0.0, 0.0, 0.0 };
float RotMat[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

// Ű������ ����ü
typedef struct frame {
	GVec3 Pos;
	GQuater Qua;
}frame;

// Callback �Լ�
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void Timer(int id);

// ����� �����Լ�
void InitGL();
void GetSphereCoord(int x, int y, GVec3 &p);
void Render();
void RenderFloor();
void SetupViewVolume();
void SetupViewTransform();
void get_interp_crv_hermite(vector<frame>& Keyframe, std::vector<GBzrCrv>& CrvList);

// Ű������ ������ ����
vector<frame> Keyframe; // Ű������ ������ ����
GVec3 P; // ��ġ
GQuater Q; // �ڼ�
double Time = 0.0;

// ������ � ���� ����
vector<GBzrCrv> gCrvList; // ������ ��� ����Ʈ
int NumPoints;
vector<double> Params; // �� ������ŭ �Ķ���͸� ������ ���� ����

int main(int argc, char **argv)
{
	// GLUT �ʱ�ȭ
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// ������ �ʱ�ȭ �� ����
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Keyframe Example");
	InitGL();
	
	// Ű������ ����
	{
		frame temp;
		// P0
		temp.Pos.Set(-8.0, 8.0, 0.0);
		temp.Qua.SetIdentity();
		Keyframe.push_back(temp);
		
		// P1
		temp.Pos.Set(-2.0, 1.5, 0.0);
		temp.Qua.SetFromAngleAxis(-120.0, GVec3(0.0, 0.0, 1.0)); 
		Keyframe.push_back(temp);

		// P2
		temp.Pos.Set(0.0, 2.0, 0.0);
		temp.Qua.SetFromAngleAxis(-170.0, GVec3(0.0, 0.0, 1.0)); 
		Keyframe.push_back(temp);

		// P3
		temp.Pos.Set(2.0, 1.4, 0.0);
		temp.Qua.SetFromAngleAxis(-220.0, GVec3(0.0, 0.0, 1.0)); 
		Keyframe.push_back(temp);

		// P4
		temp.Pos.Set(2.8, 1.5, 0.0);
		temp.Qua.SetFromAngleAxis(-250.0, GVec3(0.0, 0.0, 1.0));
		Keyframe.push_back(temp);

		// P5
		temp.Pos.Set(3.6, 1.0, 0.0);
		temp.Qua.SetFromAngleAxis(-270.0, GVec3(0.0, 0.0, 1.0)); 
		Keyframe.push_back(temp);
	}

	// �Է��� ���鿡 ���� ���� �Ķ���� ����
	NumPoints = (int)Keyframe.size();
	Params.push_back(0.0);
	for (int i = 1; i < NumPoints; ++i) 
		Params.push_back(Params[i - 1] + dist(Keyframe[i].Pos, Keyframe[i - 1].Pos));
	
	// ������ ���� ��� ���Ѵ�.
	get_interp_crv_hermite(Keyframe, gCrvList);

	// Callback �Լ� ���
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(30, Timer, 0);

	// �޽��� ��Ǫ ����
	glutMainLoop();
	return 0;
}

void InitGL()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
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

	// Ű������ �׸���
	glPushMatrix();
	{
		// �̵� ��ȯ�� �����Ѵ�.
		glTranslated(P[0], P[1], P[2]); // P0, P1 ���� ���

		// ȸ�� ��ȯ�� �����Ѵ�.
		GVec3 Axis;
		double angle;
		Q.GetAngleAxis(Axis, angle);
		glRotated(angle, Axis[0], Axis[1], Axis[2]);

		// ��ü�� ������ �Ѵ�.
		glColor3f(0.0, 0.5, 0.9);
		glutSolidCube(2);
	}
	glPopMatrix();

	// Į�� ���۸� ��ȯ�Ѵ�.
	glutSwapBuffers();
}

void Timer(int id)
{
	GVec3  V;
	// ����ð��� ���� ��ġ�� ȸ������ ����
	// ��ġ�� ���� ���� ���� - Linear interpolation
	int i = 0;
	// ������ �����̴� �ӵ� �ٸ��� ����
	if (Time <= gCrvList[0].m_Domain[1]) {
		i = 0;
		Time += 0.48;
	}
	else if (Time <= gCrvList[1].m_Domain[1]) {
		i = 1;
		Time += 0.20;
	}
	else if (Time <= gCrvList[2].m_Domain[1]) {
		i = 2;
		Time += 0.17;
	}
	else if (Time <= gCrvList[3].m_Domain[1]) {
		i = 3;
		Time += 0.12;
	}
	else if (Time <= gCrvList[4].m_Domain[1]) {
		i = 4;
		Time += 0.07;
	}
	else
		Time = 0.0;

	// ��ġ ����
	// ù��° ������ �����Ÿ� ����
	if (i == 0)
		P = ((gCrvList[0].m_Domain[1] - Time) * Keyframe[0].Pos + Time * Keyframe[1].Pos) / gCrvList[0].m_Domain[1];
	// �ι�° ���ʹ� ��� ���ö��� ����
	else
		gCrvList[i].Eval(Time, P, V);


	// ȸ���� ���� ���� ���� - Spherical linear interpolation
	double T = (Time - Params[i]) / (Params[i + 1] - Params[i]);
	Q = slerp(Keyframe[i].Qua, Keyframe[i+1].Qua, T); 

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 0);
}

void get_interp_crv_hermite(vector<frame>& Keyframe, vector<GBzrCrv>& CrvList)
{
	// �����͸� �����Ѵ�.
	GVec3 zero(0.0, 0.0, 0.0); // ������
	vector<GVec3> TanVec(NumPoints, zero); // �ʱⰪ �������� �������� �迭 ����

	// ���� ���� ���ϱ�
	for (int i = 1; i < NumPoints - 1; ++i)
		TanVec[i] = (Keyframe[i + 1].Pos - Keyframe[i - 1].Pos) / (Params[i + 1] - Params[i - 1]);

	// �� ������ ������ ��� �����Ͽ� ����Ʈ�� �߰��Ѵ�.
	vector<GVec3> CtrPts(4, GVec3()); // 3�� hermite ��� ����� ���� �������� ������ vector
	for (int i = 0; i < NumPoints - 1; ++i) {
		// ��� ���ǿ�
		GVec2 Domain = GVec2(Params[i], Params[i + 1]);

		// ������ ���ϱ�			
		CtrPts[0] = Keyframe[i].Pos; // 1�� ������
		CtrPts[1] = Keyframe[i].Pos + ((Domain[1] - Domain[0]) / 3) * TanVec[i]; // 2�� ������
		CtrPts[3] = Keyframe[i+1].Pos; // 4�� ������
		CtrPts[2] = Keyframe[i+1].Pos - ((Domain[1] - Domain[0]) / 3) * TanVec[i + 1]; // 3�� ������

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
	glDisable(GL_LIGHTING);
	glColor3d(0.0, 0.7, 0.0);
	for (int i = -20; i <= 20; ++i)
	{
		if (i == 0)
			glLineWidth(2.0);
		else
			glLineWidth(1.0);
		glBegin(GL_LINES);
		glVertex3i(i, -0.001, -20);
		glVertex3f(i, -0.001, 20);
		glVertex3i(-20, -0.001, i);
		glVertex3i(20, -0.001, i);
		glEnd();
	}
	glEnable(GL_LIGHTING);
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
		else if (button == GLUT_MIDDLE_BUTTON)
			ManipulateMode = 2;		// �̵�
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
		if (norm(Axis) < 1.0e-6)
		{
			Axis.Set(0.0, 0.0, 0.0);
			Angle = 0.0;
		}
		else
		{
			Axis.Normalize();
			Angle = p * q;
			Angle = acos(Angle) * 180.0f / 3.141592f;
		}
	}
	else  if (ManipulateMode == 2) // �̵� ���
	{
		printf("%f %f\n", x, y);
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

void MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		Zoom -= (StartPt[1] - y) * 0.05;
		if (Zoom > -0.001)
			Zoom = -0.001;	
	}
	else
	{
		Zoom += (StartPt[1] - y) * 0.05;
		if (Zoom > -0.001)
			Zoom = -0.001;
	}
	glutPostRedisplay();
}

void GetSphereCoord(int x, int y, GVec3 &p)
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

