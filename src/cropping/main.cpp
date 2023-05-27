/*
 * title: 实验三 裁剪
 * author: xwwwb
 * others: 无
 */
#include <GL/glut.h>
#include <iostream>

using namespace std;

const int x_min = 300, x_max = 700, y_min = 100, y_max = 700;

int methods = 0; // 0:cohen_sutherland 1:liang_barsky

// 初始化OpenGL
void init(int argc, char** argv);
// 绘制矩形 用于裁剪
void drawRectangle();
// 鼠标事件
void handleMouse(int button, int state, int x, int y);
// 线段的端点
int a = 20000, b = 20000, c = 20000, d = 20000;
// 画线
void drawLine();
void cohen_sutherland(bool); // cohen_sutherland算法
void liang_barsky();
int liang_barsky_test(float p, float q, float& umax, float& umin);

// 获得编码
int getCode(int x, int y)
{
	int d0 = 0, d1 = 0, d2 = 0, d3 = 0;
	if (x < x_min)
	{
		d0 = 1;
	}
	if (x > x_max)
	{
		d1 = 1;
	}
	if (y > y_max)
	{
		d2 = 1;
	}
	if (y < y_min)
	{
		d3 = 1;
	}
	return d0 * 8 + d1 * 4 + d2 * 2 + d3;
}

int main(int argc, char** argv)
{
	cout << "计算机图形学实验三\n直线的裁剪\n作者：徐文博542010010220\n使用了liang_barsky算法和cohen_sutherland算法\n使用鼠标右键切换具体算法\n"
		<< endl;
	init(argc, argv);
}

void init(int argc, char** argv)
{
	// 初始化freeGLUT
	glutInit(&argc, argv);
	// 显示模式 GLUT_RGB 颜色模式为RGB
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// 窗口大小
	glutInitWindowSize(1000, 800);
	// 窗口初始位置
	glutInitWindowPosition(500, 500);
	glutCreateWindow("Cropping using OpenGL");
	// 背景白色
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1000, 800, 0);

	glPointSize(4.0);
	glutDisplayFunc(drawRectangle);
	glutMouseFunc(handleMouse);
	glutMainLoop();
}

void drawRectangle()
{
	// 设置绘制颜色为浅绿色
	glColor3f(4.0f / 255.0f, 170.0f / 255.0f, 109.0f / 255.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(x_min, y_min);
	glVertex2i(x_max, y_min);
	glVertex2i(x_max, y_max);
	glVertex2i(x_min, y_max);
	glEnd();
	glFlush();
}

void handleMouse(int button, int state, int x, int y)
{
	// 按下鼠标左键 记录圆心坐标
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		a = x;
		b = y;
		cout << "线段起点坐标为：(" << a << "," << b << ")" << endl;
	}
	// 松开鼠标左键 记录半径
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		c = x;
		d = y;
		cout << "线段终点坐标为：(" << c << "," << d << ")" << endl;
		drawLine();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		a = 20000;
		b = 20000;
		c = 20000;
		d = 20000;
		// 切换算法
		methods = (methods + 1) % 2;
		cout << "切换算法为：" << (methods == 0 ? "cohen_sutherland" : "liang_barsky") << endl;
		if (methods == 1)
		{
			glutSetWindowTitle("liang_barsky");
		}
		else
		{
			glutSetWindowTitle("cohen_sutherland");
		}
	}
}

void drawLine()
{
	if (methods == 0)
	{
		cohen_sutherland(false);
	}
	else
	{
		liang_barsky();
	}
}

void liang_barsky()
{
	int temp1 = a, temp2 = b, temp3 = c, temp4 = d;
	float umax = 1, umin = 0;
	float dx = c - a, dy = d - b;
	if (liang_barsky_test(-dx, a - x_min, umax, umin) &&
		liang_barsky_test(dx, x_max - a, umax, umin) &&
		liang_barsky_test(-dy, b - y_min, umax, umin) &&
		liang_barsky_test(dy, y_max - b, umax, umin))
	{
		if (umax < 1)
		{
			c = a + umax * dx;
			d = b + umax * dy;
		}
		if (umin > 0)
		{
			a = a + umin * dx;
			b = b + umin * dy;
		}
		cout << "裁剪后的线段为：(" << a << "," << b << ")到(" << c << "," << d << ")" << endl;
		glColor3f(80.0f / 255.0f, 157.0f / 255.0f, 239.0f / 255.0f);
		glBegin(GL_LINES);
		glVertex2i(a, b);
		glVertex2i(c, d);
		glEnd();

		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glBegin(GL_LINES);
		glVertex2i(temp1, temp2);
		glVertex2i(a, b);
		glEnd();

		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glBegin(GL_LINES);
		glVertex2i(temp3, temp4);
		glVertex2i(c, d);
		glEnd();

		glFlush();
	}
	else
	{
		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glBegin(GL_LINES);
		glVertex2i(a, b);
		glVertex2i(c, d);
		glEnd();
		glFlush();
	}
}
int liang_barsky_test(float p, float q, float& umax, float& umin)
{

	float r = q / p;
	if (p < 0)
	{
		if (r > umax)
		{
			return 0;
		}
		else if (r > umin)
		{
			umin = r;
		}
	}
	else if (p > 0)
	{
		if (r < umin)
		{
			return 0;
		}
		else if (r < umax)
		{
			umax = r;
		}
	}
	else if (q < 0)
	{
		return 0;
	}
	return 1;
}

void cohen_sutherland(bool Iter)
{
	glBegin(GL_LINES);
	int code1 = getCode(a, b);
	int code2 = getCode(c, d);
	if ((code1 | code2) == 0)
	{
		cout << "完全在矩形内" << endl;
		glColor3f(80.0f / 255.0f, 157.0f / 255.0f, 239.0f / 255.0f);
		glVertex2i(a, b);
		glVertex2i(c, d);
	}
	else if ((code1 & code2) != 0)
	{
		cout << "完全在矩形外" << endl;
		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glVertex2i(a, b);
		glVertex2i(c, d);
	}

	else if (code1 == 0 || code2 == 0)
	{
		cout << "部分在矩形内" << endl;
		// 若p1在矩形内，p2在矩形外 则交换p1 p2
		GLfloat k = (GLfloat)(d - b) / (GLfloat)(c - a);
		if (code1 == 0)
		{
			int temp = a;
			a = c;
			c = temp;
			temp = b;
			b = d;
			d = temp;
		}
		// 交换后p1在矩形外，p2在矩形内
		// 从p1开始裁剪
		int code = getCode(a, b);
		int p1_x = a, p1_y = b; // 记录下原有的p1
		GLfloat x, y;
		cout << "p1code = " << code << endl;
		if (code & 8)
		{
			// p1在矩形左侧
			x = x_min;
			y = b + (x - a) * k;
		}
		else if (code & 4)
		{
			// p1在矩形右侧
			x = x_max;
			y = b + (x - a) * k;
		}
		else if (code & 2)
		{
			// p1在矩形下侧
			y = y_max;
			x = a + (y - b) / k;
		}
		else
		{
			// p1在矩形上侧
			y = y_min;
			x = a + (y - b) / k;
		}
		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glVertex2i(p1_x, p1_y);
		glVertex2f(x, y);
		glColor3f(80.0f / 255.0f, 157.0f / 255.0f, 239.0f / 255.0f);
		glVertex2f(x, y);
		glVertex2i(c, d);
	}
	else
	{
		cout << "P1 P2均在外部" << endl;
		GLfloat k = (GLfloat)(d - b) / (GLfloat)(c - a);
		GLfloat x_1, y_1;
		if (code1 & 8)
		{
			// p1在矩形左侧
			x_1 = x_min;
			y_1 = b + (x_1 - a) * k;
		}
		else if (code1 & 4)
		{
			// p1在矩形右侧
			x_1 = x_max;
			y_1 = b + (x_1 - a) * k;
		}
		else if (code1 & 2)
		{
			// p1在矩形下侧
			y_1 = y_max;
			x_1 = a + (y_1 - b) / k;
		}
		else
		{
			// p1在矩形上侧
			y_1 = y_min;
			x_1 = a + (y_1 - b) / k;
		}

		GLfloat x_2, y_2;
		// 计算p2 到交点的线段
		int code = getCode(c, d);
		if (code & 8)
		{
			// p2在矩形左侧
			x_2 = x_min;
			y_2 = d + (x_2 - c) * k;
		}
		else if (code & 4)
		{
			// p2在矩形右侧
			x_2 = x_max;
			y_2 = d + (x_2 - c) * k;
		}
		else if (code & 2)
		{
			// p2在矩形下侧
			y_2 = y_max;
			x_2 = c + (y_2 - d) / k;
		}
		else
		{
			// p2在矩形上侧
			y_2 = y_min;
			x_2 = c + (y_2 - d) / k;
		}

		// 绘制p1到交点的线段
		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glVertex2i(a, b);
		glVertex2f(x_1, y_1);
		// 绘制p2到交点的线段
		glColor3f(239.0f / 255.0f, 83.0f / 255.0f, 80.0f / 255.0f);
		glVertex2f(x_2, y_2);
		glVertex2i(c, d);
		a = int(x_1);
		b = int(y_1);
		c = int(x_2);
		d = int(y_2);
		if (!Iter)
		{
			cohen_sutherland(true);
		}
	}

	glEnd();
	glFlush();
}

