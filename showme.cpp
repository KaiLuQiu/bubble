#include<windows.h>
#include<stdio.h>
#include<glaux.h>
#include<GL/glut.h>
#include <math.h>
#include "tricall.h"
#include "triangle.c"
#define  Pi 3.1415926
HGLRC hRC=NULL;
HDC hDC=NULL;
HWND hWnd=NULL;
HINSTANCE hInstance;

bool keys[256];
bool active=TRUE;
bool fullscreen=TRUE;


const GLuint num=50;
GLuint loop;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
GLvoid KillGLWindow(GLvoid);
BOOL CreateGLWindow(char *title,int width, int height,int bits,bool fullscreenflag);


GLvoid ReSizeGLScene(GLsizei width,GLsizei height)
{
	
	if(height==0)
	{
		height=1;
	}
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
int	InitGL(GLvoid)
{	glShadeModel(GL_SMOOTH);	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-1.0f,1.0f,-1.0f,1.0f);

	return TRUE;
}

int DrawGLLine(GLdouble hx,GLdouble hy,GLdouble ex,GLdouble ey)
{
			
	glBegin(GL_LINES);
		glVertex2f(hx,hy);
		glVertex2f(ex,ey);
		
	glEnd();
	return TRUE;
}
int DrawGLTriangle(GLdouble hx,GLdouble hy,GLdouble mx,GLdouble my,GLdouble ex,GLdouble ey)
{
	glColor3f(0.4f,0.0f,0.0f);
	DrawGLLine(hx,hy,mx,my);
	DrawGLLine(hx,hy,ex,ey);
	DrawGLLine(mx,my,ex,ey);
		
	return TRUE;								//  һ�� OK
}
int DrawGLCircle(GLdouble hx,GLdouble hy,GLdouble R)
{
	glColor3f(0.0f,0.0f,1.0f);
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<1000; ++i)
		glVertex2f(hx+R*cos(2*Pi/1000*i), hy+R*sin(2*Pi/1000*i));
	glEnd();
	return TRUE;
}
BOOL GetResult(BubbleList bub)
{
	triangulateio in;
	triangulateio mid;
	triangulateio vorout;
	initIOV(in,bub,mid,vorout);
	triangulate("pczAevn", &in, &mid, &vorout);	

	if(mid.trianglelist!=NULL)
	{
		for (int i=0;i<mid.numberoftriangles;i++)
		{
			DrawGLTriangle(mid.pointlist[2*mid.trianglelist[i*3]],mid.pointlist[2*mid.trianglelist[i*3]+1],
				mid.pointlist[2*mid.trianglelist[i*3+1]],mid.pointlist[2*mid.trianglelist[i*3+1]+1],
				mid.pointlist[2*mid.trianglelist[i*3+2]],mid.pointlist[2*mid.trianglelist[i*3+2]+1]);

		}

		for(int i=0;i<mid.numberofpoints;i++)
		{
		
			DrawGLCircle(mid.pointlist[i*2],mid.pointlist[i*2+1],mid.pointattributelist[i]);
		}

		

	}
	
	for(int i=0;i<vorout.numberofedges;i++)
	{

		if(vorout.edgelist[i*2+1]!=-1)
		{glColor3f(0.0f,0.8f,0.0f); 
		DrawGLLine(vorout.pointlist[2*vorout.edgelist[i*2]],vorout.pointlist[2*vorout.edgelist[i*2]+1],
			vorout.pointlist[2*vorout.edgelist[i*2+1]],vorout.pointlist[2*vorout.edgelist[i*2+1]+1]);
		}
		else{
			glColor3f(0.0f,0.8f,0.0f);
		 DrawGLLine(vorout.pointlist[2*vorout.edgelist[i*2]],vorout.pointlist[2*vorout.edgelist[i*2]+1],
				vorout.pointlist[2*vorout.edgelist[i*2]]+10*vorout.normlist[i*2],vorout.pointlist[2*vorout.edgelist[i*2]+1]+10*vorout.normlist[i*2+1]);
		}
		
	}
	return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
	if(fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);

	}
	if(hRC)
	{
		if(!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,"�ͷ�DC��RCʧ��","�رմ���",MB_OK|MB_ICONINFORMATION);
		}
		if(!wglDeleteContext(hRC))
		{
			MessageBox(NULL,"�ͷ�RCʧ��","�رմ���",MB_OK|MB_ICONINFORMATION);
		}
		hRC=NULL;

	}
	if(hWnd&&!DestroyWindow(hWnd))
	{
		MessageBox(NULL,"�ͷŴ��ھ��ʧ��","�رմ���",MB_OK|MB_ICONINFORMATION);
		hWnd=NULL;
	}
	if(!UnregisterClass("OpenG",hInstance))
	{
		MessageBox(NULL,"����ע��������","�رմ���",MB_OK|MB_ICONINFORMATION);
		hInstance=NULL;
	}

}

BOOL CreateGLWindow(char *title,int width, int height,int bits,bool fullscreenflag)
{
	GLuint PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;

	RECT WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;

	fullscreen=fullscreenflag;

	hInstance=GetModuleHandle(NULL);
	wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.lpfnWndProc=(WNDPROC)WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=NULL;
	wc.lpszMenuName=NULL;
	wc.lpszClassName="OpenG";
	if(!RegisterClass(&wc))
	{
		MessageBox(NULL,"ע�ᴰ��ʧ��","�رմ���",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	if(fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth=width;
		dmScreenSettings.dmPelsHeight=height;
		dmScreenSettings.dmBitsPerPel=bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if(MessageBox(NULL,"ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��","NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;
			}
			else
			{
				MessageBox(NULL,"","",MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}

	}
	if(fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		ShowCursor(FALSE);

	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;
	}
	AdjustWindowRectEx(&WindowRect,dwStyle,FALSE,dwExStyle);
	if(!(hWnd=CreateWindowEx(dwExStyle,
		"OpenG",
		title,
		WS_CLIPSIBLINGS|
		WS_CLIPCHILDREN|
		dwStyle,
		0,0,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		KillGLWindow();
		MessageBox(NULL,"","",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;

	}
	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|
		PFD_SUPPORT_OPENGL|
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0

	};
	if(!(hDC=GetDC(hWnd)))
	{
		KillGLWindow();
		MessageBox(NULL,"���ܴ���һ����ƥ������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))
	{
		KillGLWindow();
		MessageBox(NULL," �����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;

	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))
	{
		KillGLWindow();
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return  FALSE;
	}

	if(!(hRC=wglCreateContext(hDC)))
	{
		KillGLWindow();
		MessageBox(NULL,"���ܴ���opengl��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;

	}
	if(!wglMakeCurrent(hDC,hRC))
	{
		KillGLWindow();
		MessageBox(NULL,"���ܼ���opengl��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
//	ReSizeGLScene(width,height);
	if(!InitGL())
	{
		KillGLWindow();
		MessageBox(NULL,"initialization failed","error",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;

}

LRESULT CALLBACK WndProc( HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ACTIVATE:
		{
			if(!HIWORD(wParam))
			{
				active=TRUE;
			}
			else
			{
				active=FALSE;
			}
			return 0;
		}
	case WM_SYSCOMMAND:						// ϵͳ�ж�����
		{
			switch (wParam)						// ���ϵͳ����
			{
			case SC_SCREENSAVE:				// ����Ҫ����?
			case SC_MONITORPOWER:				// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;					// ��ֹ����
			}
			break;							// �˳�
		}
	case WM_CLOSE:							// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);					// �����˳���Ϣ
			return 0;						// ����
		}



	case WM_KEYDOWN:						// �м�����ô?
		{
			keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
			return 0;						// ����
		}
	case WM_KEYUP:							// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
			return 0;						// ����
		}
	case WM_SIZE:							// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			return 0;						// ����
		}

	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);



}


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow 
	)
{
	MSG msg;
	BOOL  done=FALSE;

	fullscreen=FALSE;

	if(!CreateGLWindow("bo",700,700,16,fullscreen))
	{
		return 0;
	}
	while(!done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
			{
				done=TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
		}
		else
		{
			if(active)
			{
				if(keys[VK_ESCAPE])
				{
					done=TRUE;
				}
				else
				{
					
					glClear(GL_COLOR_BUFFER_BIT);			// �����Ļ����Ȼ���
					glLoadIdentity();							// ���õ�ǰ��ģ�͹۲����
					BubbleList bub;
					bub.Bubblelist();
					bub.Getpoint();

					GetResult(bub);
				
					
					
					SwapBuffers(hDC);
					


				}
			}
			if(keys[VK_F1])
			{
				keys[VK_F1]=FALSE;
				KillGLWindow();
				fullscreen=!fullscreen;
				if(!CreateGLWindow("bo",700,700,16,fullscreen))
				{
					return 0;
				}
			}

		}
	}
	KillGLWindow();
	return(msg.wParam);
}
