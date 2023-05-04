#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

class Point {
public :
    int x;
    int y;

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

vector<vector<Point>> *points;
vector<Point> fill_points;
GLFWwindow *window;
float xscale, yscale;

int OpenGLInit();

void mouseHandler(GLFWwindow *, int, int, int);

void keyHandler(GLFWwindow *, int, int, int, int);

void flood_fill(int x, int y);

int main() {
    OpenGLInit();
    glfwSetMouseButtonCallback(window, mouseHandler);
    glfwSetKeyCallback(window, keyHandler);
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
    glOrtho(0, 640, 480, 0, -1, 1);
    glClearColor(1, 1, 1, 1);

    points = new vector<vector<Point>>();
    points->push_back(*(new vector<Point>()));

    fill_points = *(new vector<Point>());

    GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
    glfwGetMonitorContentScale(pMonitor, &xscale, &yscale);
    return 0;
}

void drawLine() {
    for (auto points_: *points) {
        if (points_.size() <= 1 || (points_.front().x == -100 && points_.front().y == -100)) {
            break;
        }
        // #03A9F4
        glColor3f(0.01, 0.66, 0.96);
        glLineWidth(5.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 1; i < points_.size(); i++) {
            glVertex2i(points_[i].x, points_[i].y);
        }
        glEnd();
    }

}

void drawPoints() {
    for (auto points_: *points) {
        if (points_.size() <= 1) {
            break;
        }
        // #03A9F4
        glColor3f(0.01, 0.66, 0.96);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        for (auto point: points_) {
            glVertex2i(point.x, point.y);
        }
        glEnd();
    }
}

void SinglePoint(int x, int y) {
    glColor3f(0.9, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glVertex2f(x + 0.1f, y + 0.1f);
    glVertex2f(x - 0.1f, y - 0.1f);
    glVertex2f(x + 0.1f, y - 0.1f);
    glVertex2f(x - 0.1f, y + 0.1f);
    glEnd();
    glfwSwapBuffers(window);
}

void mouseHandler(GLFWwindow *window_, int button, int action, int mods) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        if (points->back().empty()) {
            points->back().emplace_back(-100, -100); // 使用-100 标记没有被绘制
            points->back().emplace_back(int(x), int(y));
        } else {
            auto *point = &points->back();
            point->push_back(Point(int(x), int(y)));
        }

    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (points->back().size() > 1) {
            points->back().front().x = -10; // 使用-10来标记 需要被绘制
            points->back().front().y = -10;
            points->push_back(*(new vector<Point>()));
        }
    }
    drawPoints();
    drawLine();
    glfwSwapBuffers(window_);
}

void keyHandler(GLFWwindow *window_, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        // 拿到鼠标位置
        double xpos, ypos;
        glfwGetCursorPos(window_, &xpos, &ypos);
        flood_fill(int(xpos), int(ypos));
    }
}

void flood_fill(int xpos, int ypos) {
    stack<Point *> fill_stack;
    fill_stack.push(new Point(xpos, ypos));
    // 保存当前像素
    unsigned char pixel[3];

    while (!fill_stack.empty()) {
        Point *point = fill_stack.top();
        fill_stack.pop();
        int x = point->x;
        int y = point->y;
        delete point;
        if (x <= 1 || x >= 639 || y <= 1 || y >= 479) {
            continue;
        }

        int x_ = int(double(x) * xscale);
        int y_ = int(double(480 - y) * yscale);
        int w = int(1 * xscale);
        int h = int(1 * yscale);

        glReadPixels(x_, y_, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixel);

//        if (pixel[0] == 3 && pixel[1] == 168 && pixel[2] == 245) {
//            continue;
//        }
        if (pixel[0] != 255 && pixel[1] != 255 && pixel[2] != 255) {
            continue;
        }
        SinglePoint(x, y);
        fill_stack.push(new Point(x + 2, y));
//        fill_stack.push(new Point(x + 1, y + 1));
        fill_stack.push(new Point(x - 2, y));
//        fill_stack.push(new Point(x + 1, y - 1));
        fill_stack.push(new Point(x, y + 2));
//        fill_stack.push(new Point(x - 1, y + 1));
        fill_stack.push(new Point(x, y - 2));
//        fill_stack.push(new Point(x - 1, y - 1));
        cout << "flood fill" << endl;
    }
}