#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
GLFWwindow *window;
int OpenGLInit();

class Point{
public :
    int x;
    int y;
    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
};
vector<Point> points;
bool drawLineFlag = false;
void drawLine()
{
   if(!drawLineFlag){
       glClear(GL_COLOR_BUFFER_BIT);
       if(points.size() > 1){
           glBegin(GL_LINE_STRIP);
           glColor3b(0,0,0);
           for(int i = 0; i < points.size(); i++){
               glVertex2i(points[i].x, points[i].y);
           }
           glEnd();

       }
   }else{
         glClear(GL_COLOR_BUFFER_BIT);
         if(points.size() > 1){
              glBegin(GL_LINE_STRIP);
              glColor3b(0,0,0);
              for(int i = 0; i < points.size(); i++){
                glVertex2i(points[i].x, points[i].y);
              }
              glEnd();
              glFlush();
         }
         glBegin(GL_LINE_STRIP);
         glColor3b(0,0,0);
         glVertex2i(points[points.size() - 1].x, points[points.size() - 1].y);
         glVertex2i(points[0].x, points[0].y);
         glEnd();
      
   }

}
int main(){
    OpenGLInit();

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods){
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            std::cout<<"x:"<<x<<" y:"<<y<<std::endl;
            points.push_back(Point(int(x), int(y)));
        }
        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
            drawLineFlag = true;
        }
    });


    while(!glfwWindowShouldClose(window)){
        drawLine();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


int OpenGLInit(){
    if(!glfwInit()){
        return -1;
    }
    // 创建一个窗口
    window = glfwCreateWindow(640, 480, "Learn OpenGL", NULL, NULL);

    if(!window){
        glfwTerminate();
        return -1;
    }
    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    // 设置坐标系
    glOrtho(0, 640, 480, 0, -1, 1);
    glClearColor(1, 1, 1, 1);
}