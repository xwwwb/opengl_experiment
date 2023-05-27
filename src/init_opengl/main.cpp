#include <iostream>
#include <GL/glut.h>

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRectf(-0.5f, -0.5f, 0.5f, 0.5f);
    glFlush();

}

void display()
{
    glClearColor(0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glRotatef(45, 1.0f, 1.0f, 1.0f);
    glutWireTeapot(0.5f);
    glFlush();
}


void lines()
{
    int p1[] = { 60,40 };
    int p2[] = { 10,10 };
    int p3[] = { 30,70 };
    int p4[] = { 50,10 };
    int p5[] = { 2,40 };
    glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0,0);
    glPointSize(10);//一个点占据三个像素
	glBegin(GL_LINE_LOOP);
	
    glVertex2iv(p1);
    glVertex2iv(p2);
    glVertex2iv(p3);
    glVertex2iv(p4);
    glVertex2iv(p5);

	glEnd();
	glFlush();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glEnable(GL_DEPTH_TEST);
    glutCreateWindow("OpenGL程序");
    gluOrtho2D(0, 400, 0, 400);

   // gluOrtho2D(-100, 100, -100, 100 );
    // 这四个参数代表 窗口的四个边分别是
    // 左 y = -100 
    // 右 y = 100
    // 下 x = -100
    // 上 x = 100

    glutDisplayFunc(&lines);
    glutMainLoop();
    return 0;

}