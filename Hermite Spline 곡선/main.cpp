#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "gmath.h"
#include "bzrcrv.h"
#include ".\usr\include\GL\freeglut.h"

using namespace std;
int Width = 800, Height = 800;
int ManipulateMode = 0;

// 관측변환을 위한 변수
int StartPt[2];
float Angle = 0.0;
GVec3 Axis;
float Zoom = -50.0;
float Pan[3] = { 0.0, 0.0, 0.0 };
float RotMat[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
double gTime = 0.0;

// 보간 데이터의 초기값
std::vector<GVec3> gPoints;
std::vector<GBzrCrv> gCrvList; // 베지에 곡선의 리스트

// Callback 함수
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);
void Timer(int id);

// 사용자 정의함수
void GetSphereCoord(int x, int y, GVec3& p);
void Render();
void RenderFloor();
void SetupViewVolume();
void SetupViewTransform();
void get_interp_crv_hermite(std::vector<GVec3>& Points, std::vector<GBzrCrv>& CrvList);

int main(int argc, char** argv)
{
	// GLUT 초기화
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// 윈도우 초기화 및 생성
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Bezier Curves");

	// 보간 정점 설정
	gPoints.push_back(GVec3(0.0, 5.0, 0.0));
	gPoints.push_back(GVec3(3.5, 7.0, 0.0));
	gPoints.push_back(GVec3(7.0, 5.0, 0.0));
	gPoints.push_back(GVec3(5.0, 0.0, 0.0));
	gPoints.push_back(GVec3(0.0, -5.0, 0.0));
	gPoints.push_back(GVec3(0.0, 5.0, 0.0));

	// 베지에 보간 곡선을 구한다.
	get_interp_crv_hermite(gPoints, gCrvList);

	// Callback 함수 등록
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutKeyboardFunc(Keyboard);
	//glutTimerFunc(30, Timer, 0);

	// 메시지 루푸 진입
	glutMainLoop();
	return 0;
}

void Render()
{
	// 칼라 버퍼와 깊이 버퍼를 초기화 한다.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 관측 공간을 설정하고, 뷰잉 변환을 수행한다.
	SetupViewVolume();
	SetupViewTransform();

	// 모델 리스트를 렌더링한다.
	glMatrixMode(GL_MODELVIEW);
	RenderFloor();

	// 보간점을 렌더링 한다.
	glColor3f(1.0, 0.0, 0.0); // 빨간색.
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < gPoints.size(); ++i)
		glVertex3d(gPoints[i][0], gPoints[i][1], gPoints[i][2]);
	glEnd();
	glPointSize(1.0);

	for (int i = 0; i < gCrvList.size(); ++i)
		gCrvList[i].Render(50);

	// 칼라 버퍼를 교환한다.
	glutSwapBuffers();
}

/*!
*	\brief	정점을 보간하는 3차 베지에 곡선의 리스트를 생성한다.
*
*	\param	Points[in]		보간될 데이터 정점
*	\param	CrvList[out]	생성된 베지에 곡선이 저장된다.
*/
void get_interp_crv_hermite(std::vector<GVec3>& Points, std::vector<GBzrCrv>& CrvList)
{
	// 1. 보간 파라미터를 결정한다. -> chord length 파라미터
	int NumPoints = (int)Points.size();
	vector<double> Params(NumPoints, 0.0); // 점 개수만큼 파라미터를 저장할 벡터 생성
	for (int i = 1; i < NumPoints; ++i)
		Params[i] = Params[i - 1] + dist(Points[i], Points[i - 1]);

	// 2. 접벡터를 결정한다.
	GVec3 zero(0.0, 0.0, 0.0); // 영벡터
	vector<GVec3> TanVec(NumPoints, zero); // 초기값 영벡터인 접선벡터 배열 생성

	//접선 벡터 구하기
	for (int i = 1; i < NumPoints-1 ; ++i)
		TanVec[i] = (Points[i + 1] - Points[i - 1]) / (Params[i + 1] - Params[i - 1]);

	// 마지막 곡선을 그리기 위한 법선벡터 구하기
	if (Points[0] == Points[NumPoints - 1])
		TanVec[0] = TanVec[NumPoints - 1] = (Points[1] - Points[NumPoints - 2]) / dist(Points[1], Points[NumPoints - 2]);

	// 3. 각 구간의 베지에 곡선을 생성하여 리스트에 추가한다.
	vector<GVec3> CtrPts(4, GVec3()); // 3차 hermite 곡선을 만들기 위한 제어점을 저장할 vector
	for (int i = 0; i < NumPoints-1; ++i) {
		// 곡선의 정의역
	    GVec2 Domain = GVec2(Params[i], Params[i + 1]);

		// 제어점 구하기			
		CtrPts[0] = Points[i]; // 1번 제어점
		CtrPts[1] = Points[i] + ((Domain[1] - Domain[0]) / 3) * TanVec[i]; // 2번 제어점
		CtrPts[3] = Points[i + 1]; // 4번 제어점
		CtrPts[2] = Points[i + 1] - ((Domain[1] - Domain[0]) / 3) * TanVec[i + 1]; // 3번 제어점
		
		// 곡선 만들기
		CrvList.push_back(GBzrCrv(Domain, CtrPts));
	}
}

void Reshape(int w, int h)
{
	// 뷰포트 변환을 수행한다.
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void Keyboard(unsigned char key, int x, int y)
{
	// ESC 키를 누르면 메쉬 리스트를 메모리에서 삭제하고 종료한다.
	if (key == 27)
		exit(0);
}

void SetupViewTransform()
{
	// 모델뷰 행렬 선택
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 이동-회전 변환 수행
	glTranslatef(0.0, 0.0, Zoom);
	glRotatef(Angle, Axis[0], Axis[1], Axis[2]);

	// 이전에 적용된 회전 변환 후, 이동 변환 수행
	glMultMatrixf(RotMat);
	glTranslatef(Pan[0], Pan[1], Pan[2]);

	// 회전 변환의 결과를 저장
	glGetFloatv(GL_MODELVIEW_MATRIX, RotMat);
	RotMat[12] = RotMat[13] = RotMat[14] = 0.0;
}

void SetupViewVolume()
{
	// 투영 행렬 선택 및 관측 공간 지정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)Width / (double)Height, 0.01, 10000.0);
}

void RenderFloor()
{
	// 바닥 평면을 렌더링한다.
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
	// 마우스 버튼을 누른 경우,
	if (state == GLUT_DOWN)
	{
		StartPt[0] = x;
		StartPt[1] = y;

		if (button == GLUT_LEFT_BUTTON)
			ManipulateMode = 1;		// 회전
		else if (button == GLUT_RIGHT_BUTTON)
			ManipulateMode = 2;		// 줌인/아웃
		else
			ManipulateMode = 3;		// 이동
	}

	// 마우스 버튼을 땐 경우,
	if (state == GLUT_UP)
	{
		// 재 초기화
		ManipulateMode = 0;
		StartPt[0] = StartPt[1] = 0;
		Angle = 0.0;
	}
}

void MouseMove(int x, int y)
{
	// 회전 모드라면,
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
	else  if (ManipulateMode == 2) // Zoom 모드라면,
	{
		Zoom += (StartPt[1] - y) * 0.1;
		if (Zoom > -0.001)
			Zoom = -0.001;
	}
	else	// 이동 모드라면,
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

