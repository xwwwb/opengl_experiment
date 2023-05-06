#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

int OpenGLInit();
void mouseHandler(GLFWwindow *window, int button, int action, int mods);
GLFWwindow *window;


int main() {
    OpenGLInit();
    glfwSetMouseButtonCallback(window, mouseHandler);
    //    glfwSetKeyCallback(window, keyHandler);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

int OpenGLInit() {
    if (!glfwInit()) {
        return -1;
    }
    // 创建一个窗口
    window = glfwCreateWindow(640, 480, "Learn OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    // 设置坐标系
    glOrtho(0, 640, 0, 480, -1, 1);
    glClearColor(1, 1, 1, 1);
    return 0;
}

void mouseHandler(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y); // 这里拿到的位置 始终以左上角为原点 不受 glOrtho的影响
        cout << "x:" << x << " y:" << y << endl;
    }
}