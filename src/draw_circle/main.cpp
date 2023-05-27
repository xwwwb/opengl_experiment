/*
 * title: 实验二 绘制圆形或者绘制椭圆形
 * author: xwwwb
 * others: 注意init的时候各个函数的执行顺序
 */

#include<GL/glut.h>
#include<iostream>
#include<cmath>

using namespace std;
// 定义PI
const double PI = 3.1415926;

int flag = 0; // 用于标记在右键的时候不能调用拖动函数

// 画圆相关
int circle_x = 20000, circle_y = 20000, circle_r = 0; // 依次是圆心的x坐标，圆心的y坐标，圆的半径
int circle_method = 1; // 1 为圆方程法 2 为极坐标法 3 为Bresenham算法
void circle_init(int argc, char** argv); // 初始化
void drawCircle(); // 切换具体算法
void drawFullCircle(int x, int y); // 补全7/8的图像和位移
void drawCircle_Equation(); // 使用圆方程法画圆
void drawCircle_Polar(); // 使用极坐标法画圆
void drawCircle_bresenham(); // 使用Bresenham算法画圆
void handleDrawCircleMouse(int button, int state, int x, int y); // 鼠标点击事件
void handleDrawCircleMouseDrag(int x, int y); // 鼠标拖动事件

// 画椭圆相关
int ellipse_a = 20000, ellipse_b = 20000, ellipse_m = 20000, ellipse_n = 20000; // 椭圆外切矩阵 对角坐标
int ellipse_method = 1; // 1 为椭圆方程 2 Bresenham算法
void drawEllipse(); // 切换具体算法
void ellipse_init(int argc, char** argv); // 初始化
void handleDrawEllipseMouse(int button, int state, int x, int y);
void handleDrawEllipseMouseDrag(int x, int y);
void drawFullEllipse(int x, int y); // 补全3/4的图像和位移
void drawEllipse_Equation(); // 使用椭圆方程法画椭圆
void drawEllipse_bresenham(); // 使用Bresenham算法画椭圆

int main(int argc, char** argv) {
	cout << "计算机图形学实验二\n绘制圆和椭圆\n作者：徐文博542010010220\n绘制圆采用了常规方程法，极坐标法，bresenham算法\n绘制椭圆采用了常规方程法和bresenham算法\n使用鼠标右键切换具体算法\n输入1画圆，其他字符画椭圆"
		<< endl;
	int result;
	cin >> result;
	if (result == 1) {
		circle_init(argc, argv);
	}
	else {
		ellipse_init(argc, argv);
	}
}

void circle_init(int argc, char** argv) {
	// 初始化freeGLUT
	glutInit(&argc, argv);
	// 显示模式 GLUT_RGB 颜色模式为RGB
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// 窗口大小
	glutInitWindowSize(1000, 800);
	// 窗口初始位置
	glutInitWindowPosition(500, 500);
	glutCreateWindow("使用OpenGL绘制圆形");
	// 背景白色
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1000, 800, 0);
	// 设置绘制颜色为浅绿色
	glColor3f(4.0f / 255.0f, 170.0f / 255.0f, 109.0f / 255.0f);
	glPointSize(4.0);
	glutDisplayFunc(drawCircle);
	glutMouseFunc(handleDrawCircleMouse);
	glutMotionFunc(handleDrawCircleMouseDrag);
	glutMainLoop();
}


void handleDrawCircleMouse(int button, int state, int x, int y) {
	// 按下鼠标左键 记录圆心坐标
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		circle_x = x;
		circle_y = y;
		circle_r = 0;
		cout << "圆心坐标为：(" << circle_x << "," << circle_y << ")" << endl;
	}
	// 松开鼠标左键 记录半径
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		// 使用勾股定理求半径
		double r = sqrt(pow(circle_x - x, 2) + pow(circle_y - y, 2));
		circle_r = int(r);

		drawCircle();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		flag = 1;
		circle_r = 1;
		circle_x = 20000;
		circle_y = 20000;
		// 切换算法
		circle_method = (circle_method + 1) % 3;
		if (circle_method == 1) {
			glutSetWindowTitle("Equation");
			cout << "使用圆方程法绘制圆形" << endl;
		}
		else if (circle_method == 2) {
			glutSetWindowTitle("Polar");
			cout << "使用极坐标法绘制圆形" << endl;
		}
		else {
			glutSetWindowTitle("Bresenham");
			cout << "使用Bresenham算法绘制圆形" << endl;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		flag = 0;
	}

}

void handleDrawCircleMouseDrag(int x, int y) {
	if (flag == 1) {
		return;
	}
	double r = sqrt(pow(circle_x - x, 2) + pow(circle_y - y, 2));
	circle_r = int(r);
	drawCircle();
}

void drawFullCircle(int x, int y) {
	// 传入的x y 是相对于圆心的坐标
	// 画八分之一的圆
	if (flag == 1) {
		return;
	}
	glBegin(GL_POINTS);

	glVertex2i(circle_x + x, circle_y + y);
	glVertex2i(circle_x + x, circle_y - y);
	glVertex2i(circle_x - x, circle_y + y);
	glVertex2i(circle_x - x, circle_y - y);
	glVertex2i(circle_x + y, circle_y + x);
	glVertex2i(circle_x + y, circle_y - x);
	glVertex2i(circle_x - y, circle_y + x);
	glVertex2i(circle_x - y, circle_y - x);

	glEnd();
	glFlush();
}

void drawCircle() {
	if (circle_method == 1) {
		drawCircle_Equation();
	}
	else if (circle_method == 2) {
		drawCircle_Polar();
	}
	else {
		drawCircle_bresenham();
	}
}
void drawCircle_Equation() {
	glClear(GL_COLOR_BUFFER_BIT);
	// 因为一次只用画1/8的圆 所以这里x有边界
	double edge = circle_r / sqrt(2);
	int edge_int = int(ceil(edge));
	int x = 0; // x的初始值为0 假设在坐标轴中心开始画
	int y = circle_r;
	while (x < edge_int + 1) {
		drawFullCircle(x, y);
		x = x + 1;
		y = int(round(sqrt(pow(circle_r, 2) - pow(x, 2))));
	}
}
void drawCircle_Polar() {
	glClear(GL_COLOR_BUFFER_BIT);
	int x = 0;
	int y = circle_r;
	double edge = circle_r / sqrt(2);
	int edge_int = int(ceil(edge));
	double dTheta = 45.0 / edge_int;
	double theta = 90;
	while (theta > 45) {
		drawFullCircle(x, y);
		theta -= dTheta;
		x = int(round(circle_r * cos(theta * PI / 180)));
		y = int(round(circle_r * sin(theta * PI / 180)));
	}
}
void drawCircle_bresenham() {
	glClear(GL_COLOR_BUFFER_BIT);
	int x = 0;
	int y = circle_r;
	int d = 1 - circle_r;
	while (x < y) {
		drawFullCircle(x, y);
		if (d < 0) {
			d = d + 2 * x + 3;
		}
		else {
			d = d + 2 * (x - y) + 5;
			y = y - 1;
		}
		x = x + 1;
	}
}


void ellipse_init(int argc, char** argv) {
	// 初始化freeGLUT
	glutInit(&argc, argv);
	// 显示模式 GLUT_RGB 颜色模式为RGB
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// 窗口大小
	glutInitWindowSize(1000, 800);
	// 窗口初始位置
	glutInitWindowPosition(500, 500);
	glutCreateWindow("使用OpenGL绘制椭圆");
	// 背景白色
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1000, 800, 0);
	// 设置绘制颜色为浅绿色
	glColor3f(4.0f / 255.0f, 170.0f / 255.0f, 109.0f / 255.0f);
	glPointSize(4.0);

	glutDisplayFunc(drawEllipse);
	glutMouseFunc(handleDrawEllipseMouse);
	glutMotionFunc(handleDrawEllipseMouseDrag);
	glutMainLoop();
}

void drawEllipse() {
	if (ellipse_method == 1) {
		drawEllipse_Equation();
	}
	else {
		drawEllipse_bresenham();
	}

}

void handleDrawEllipseMouse(int button, int state, int x, int y) {
	// 按下鼠标左键 记录圆心坐标
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		ellipse_a = x;
		ellipse_b = y;
		cout << "椭圆左上角坐标为：(" << ellipse_a << "," << ellipse_b << ")" << endl;
	}
	// 松开鼠标左键 记录半径
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		ellipse_m = x;
		ellipse_n = y;
		cout << "椭圆右下角坐标为：(" << ellipse_m << "," << ellipse_n << ")" << endl;

		drawEllipse();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		flag = 1;
		// 将椭圆参数初始化为不显示
		ellipse_a = 20000;
		ellipse_b = 20000;
		ellipse_m = 20000;
		ellipse_n = 20000;
		// 切换算法
		ellipse_method = (ellipse_method + 1) % 2;

		if (ellipse_method == 1) {
			glutSetWindowTitle("Equation");
			cout << "使用椭圆方程法绘制圆形" << endl;
		}
		else  {
			glutSetWindowTitle("Bresenham");
			cout << "使用Bresenham法绘制圆形" << endl;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		flag = 0;
	}
}

void handleDrawEllipseMouseDrag(int x, int y) {
	if (flag == 1) {
		return;
	}
	ellipse_m = x;
	ellipse_n = y;

	drawEllipse();
}

void drawFullEllipse(int x, int y) {
	// 传入的x y 是相对于圆心的坐标
	// 画3/4的圆
	if (flag == 1) {
		return;
	}
	glBegin(GL_POINTS);
	// 传入的x y 是相对于圆心的坐标 所以要加上圆心坐标
	int a = (ellipse_m + ellipse_a) / 2.0;
	int b = (ellipse_n + ellipse_b) / 2.0;
	glVertex2i(a + x, b + y);
	glVertex2i(a - x, b + y);
	glVertex2i(a + x, b - y);
	glVertex2i(a - x, b - y);

	glEnd();
	glFlush();

}

void drawEllipse_Equation() {
	glClear(GL_COLOR_BUFFER_BIT);
	// 计算出长短轴
	double a = abs(ellipse_m - ellipse_a) / 2.0;
	double b = abs(ellipse_n - ellipse_b) / 2.0;
	int x = 0;
	int y = b;
	drawFullEllipse(x, y);
	while (b * b * (x + 1) < a * a * (y - 0.5)) {
		x = x + 1; // 这里用x++ 
		y = (int)sqrt(b * b * (1.0 - x * x / (a * a)));
		drawFullEllipse(x, y);
	}
	while (y > 0) {
		y = y - 1;
		x = (int)sqrt(a * a * (1.0 - y * y / (b * b)));
		drawFullEllipse(x, y);
	}

}
void drawEllipse_bresenham() {
	glClear(GL_COLOR_BUFFER_BIT);
	// 计算出长短轴
	int a = abs(ellipse_m - ellipse_a) / 2.0;
	int b = abs(ellipse_n - ellipse_b) / 2.0;

	int x = 0;
	int y = b;
	double d1 = b * b + a * a * (-b + 0.25);
	drawFullEllipse(x, y);
	while (b * b * (x + 1) < a * a * (y - 0.5)) {
		if (d1 < 0) {
			d1 += b * b * (2 * x + 3);
		}
		else {
			d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
			y--;
		}
		x++;
		drawFullEllipse(x, y);
	}
	double d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
	while (y > 0) {
		if (d2 < 0) {
			d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
			x++;
		}
		else {
			d2 += a * a * (-2 * y + 3);
		}
		y--;
		drawFullEllipse(x, y);
	}
} 
