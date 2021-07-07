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

// 관측변환을 위한 변수
int StartPt[2];
float Angle = 0.0;
GVec3 Axis;
float Zoom = -50.0;
float Pan[3] = { 0.0, 0.0, 0.0 };
float RotMat[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

// 키프레임 구조체
typedef struct frame {
	GVec3 Pos;
	GQuater Qua;
}frame;

// Callback 함수
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void Timer(int id);

// 사용자 정의함수
void InitGL();
void GetSphereCoord(int x, int y, GVec3 &p);
void Render();
void RenderFloor();
void SetupViewVolume();
void SetupViewTransform();
void get_interp_crv_hermite(vector<frame>& Keyframe, std::vector<GBzrCrv>& CrvList);

// 키프레임 변수를 정의
vector<frame> Keyframe; // 키프레임 저장한 벡터
GVec3 P; // 위치
GQuater Q; // 자세
double Time = 0.0;

// 베지에 곡선 관련 변수
vector<GBzrCrv> gCrvList; // 베지에 곡선의 리스트
int NumPoints;
vector<double> Params; // 점 개수만큼 파라미터를 저장할 벡터 생성

int main(int argc, char **argv)
{
	// GLUT 초기화
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// 윈도우 초기화 및 생성
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Keyframe Example");
	InitGL();
	
	// 키프레임 설정
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

	// 입력한 점들에 대한 보간 파라미터 구함
	NumPoints = (int)Keyframe.size();
	Params.push_back(0.0);
	for (int i = 1; i < NumPoints; ++i) 
		Params.push_back(Params[i - 1] + dist(Keyframe[i].Pos, Keyframe[i - 1].Pos));
	
	// 베지에 보간 곡선을 구한다.
	get_interp_crv_hermite(Keyframe, gCrvList);

	// Callback 함수 등록
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(30, Timer, 0);

	// 메시지 루푸 진입
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
	// 칼라 버퍼와 깊이 버퍼를 초기화 한다.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 관측 공간을 설정하고, 뷰잉 변환을 수행한다.
	SetupViewVolume();
	SetupViewTransform();

	// 모델 리스트를 렌더링한다.
	glMatrixMode(GL_MODELVIEW);
	RenderFloor();

	// 키프레임 그리기
	glPushMatrix();
	{
		// 이동 변환을 적용한다.
		glTranslated(P[0], P[1], P[2]); // P0, P1 보간 결과

		// 회전 변환을 적용한다.
		GVec3 Axis;
		double angle;
		Q.GetAngleAxis(Axis, angle);
		glRotated(angle, Axis[0], Axis[1], Axis[2]);

		// 객체를 렌더링 한다.
		glColor3f(0.0, 0.5, 0.9);
		glutSolidCube(2);
	}
	glPopMatrix();

	// 칼라 버퍼를 교환한다.
	glutSwapBuffers();
}

void Timer(int id)
{
	GVec3  V;
	// 현재시간에 대한 위치와 회전각도 보간
	// 위치에 대한 선형 보간 - Linear interpolation
	int i = 0;
	// 구간별 움직이는 속도 다르게 구현
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

	// 위치 보간
	// 첫번째 구간은 직선거리 보간
	if (i == 0)
		P = ((gCrvList[0].m_Domain[1] - Time) * Keyframe[0].Pos + Time * Keyframe[1].Pos) / gCrvList[0].m_Domain[1];
	// 두번째 부터는 허밋 스플라인 보간
	else
		gCrvList[i].Eval(Time, P, V);


	// 회전에 대한 선형 보간 - Spherical linear interpolation
	double T = (Time - Params[i]) / (Params[i + 1] - Params[i]);
	Q = slerp(Keyframe[i].Qua, Keyframe[i+1].Qua, T); 

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 0);
}

void get_interp_crv_hermite(vector<frame>& Keyframe, vector<GBzrCrv>& CrvList)
{
	// 접벡터를 결정한다.
	GVec3 zero(0.0, 0.0, 0.0); // 영벡터
	vector<GVec3> TanVec(NumPoints, zero); // 초기값 영벡터인 접선벡터 배열 생성

	// 접선 벡터 구하기
	for (int i = 1; i < NumPoints - 1; ++i)
		TanVec[i] = (Keyframe[i + 1].Pos - Keyframe[i - 1].Pos) / (Params[i + 1] - Params[i - 1]);

	// 각 구간의 베지에 곡선을 생성하여 리스트에 추가한다.
	vector<GVec3> CtrPts(4, GVec3()); // 3차 hermite 곡선을 만들기 위한 제어점을 저장할 vector
	for (int i = 0; i < NumPoints - 1; ++i) {
		// 곡선의 정의역
		GVec2 Domain = GVec2(Params[i], Params[i + 1]);

		// 제어점 구하기			
		CtrPts[0] = Keyframe[i].Pos; // 1번 제어점
		CtrPts[1] = Keyframe[i].Pos + ((Domain[1] - Domain[0]) / 3) * TanVec[i]; // 2번 제어점
		CtrPts[3] = Keyframe[i+1].Pos; // 4번 제어점
		CtrPts[2] = Keyframe[i+1].Pos - ((Domain[1] - Domain[0]) / 3) * TanVec[i + 1]; // 3번 제어점

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
	// 마우스 버튼을 누른 경우,
	if (state == GLUT_DOWN)
	{
		StartPt[0] = x;
		StartPt[1] = y;

		if (button == GLUT_LEFT_BUTTON)
			ManipulateMode = 1;		// 회전
		else if (button == GLUT_MIDDLE_BUTTON)
			ManipulateMode = 2;		// 이동
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
	else  if (ManipulateMode == 2) // 이동 모드
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

