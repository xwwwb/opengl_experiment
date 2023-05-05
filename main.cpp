#include <GLFW/glfw3.h>
#include <iostream>
#include <stack>
#include <vector>
using namespace std;

class Point {
public:
    int x;
    int y;

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

vector<vector<Point>> *points;
GLFWwindow *window;
float xscale, yscale;

const int method = 1;// 0: 种子 1: 扫描线

int OpenGLInit();
void drawLine();
void drawPoins();
void SinglePoint(int, int);
void mouseHandler(GLFWwindow *, int, int, int);
void keyHandler(GLFWwindow *, int, int, int, int);
void read_color(int x, int y, unsigned char *pixel);
void flood_fill(int, int);
void scanning_strip(int, int);


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
    GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
    glfwGetMonitorContentScale(pMonitor, &xscale, &yscale);
    return 0;
}

void drawLine() {
    for (auto points_: *points) {
        if (points_.size() <= 1 || (points_.front().x == -100 && points_.front().y == -100)) {
            break;
        }
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
    if (!method) {
        glPointSize(5.0);
        glVertex2i(x, y);
        glVertex2f(x + 0.1f, y + 0.1f);
        glVertex2f(x - 0.1f, y - 0.1f);
        glVertex2f(x + 0.1f, y - 0.1f);
        glVertex2f(x - 0.1f, y + 0.1f);
    } else {
        glPointSize(1.0);
        glVertex2i(x, y);
    }

    glEnd();
    glfwSwapBuffers(window);
}

void mouseHandler(GLFWwindow *window_, int button, int action, int mods) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        if (points->back().empty()) {
            points->back().emplace_back(-100, -100);// 使用-100 标记没有被绘制
            points->back().emplace_back(int(x), int(y));
        } else {
            auto *point = &points->back();
            point->push_back(Point(int(x), int(y)));
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (points->back().size() > 1) {
            points->back().front().x = -10;// 使用-10来标记 需要被绘制
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
        if (method) {
            scanning_strip(int(xpos), int(ypos));
        } else {
            flood_fill(int(xpos), int(ypos));
        }
    }
}

void read_color(int x, int y, unsigned char *pixel) {
    int x_ = int(double(x) * xscale);
    int y_ = int(double(480 - y) * yscale);
    int w = int(1 * xscale);
    int h = int(1 * yscale);
    glReadPixels(x_, y_, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixel);
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
        read_color(x, y, pixel);
        if (pixel[0] != 255 && pixel[1] != 255 && pixel[2] != 255) {
            continue;
        }
        SinglePoint(x, y);
        fill_stack.push(new Point(x + 2, y));
        fill_stack.push(new Point(x - 2, y));
        fill_stack.push(new Point(x, y + 2));
        fill_stack.push(new Point(x, y - 2));
    }
}

void scanning_strip(int xpos, int ypos) {
    stack<Point *> fill_stack;
    fill_stack.push(new Point(xpos, ypos));
    unsigned char pixel[3];

    while (!fill_stack.empty()) {
        Point *point = fill_stack.top();   // 取当前栈顶元素为种子点
        fill_stack.pop();                  // 删除当前栈顶元素
        int l_x = point->x, r_x = point->x;// 向右行驶到最大的x 和向左行驶到最大的x
        while (r_x <= 640) {
            // 向右行驶
            read_color(r_x, point->y, pixel);
            if (int(pixel[0]) == 3 && int(pixel[1]) == 168 && int(pixel[2]) == 245) {
                break;
            }

            SinglePoint(r_x, point->y);
            r_x++;
        }
        while (l_x >= 0) {
            read_color(l_x, point->y, pixel);
            if (int(pixel[0]) == 3 && int(pixel[1]) == 168 && int(pixel[2]) == 245) {
                break;
            }
            SinglePoint(l_x, point->y);// 向左绘制
            l_x--;
        }

        int b_time = 0;
        int b_x = r_x, b_y = point->y + 2;
        while (b_x >= 0 && b_time < 6) {
            int result = 0;
            read_color(b_x, b_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(b_x + 1, b_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(b_x - 1, b_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(b_x + 2, b_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(b_x - 2, b_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            if (result == 5) {
                fill_stack.push(new Point(b_x, b_y));
                break;
            }
            b_x -= 1;// 向右寻找可用点
            b_time++;
        }
        // 向上寻找
        int u_time = 0;
        int u_x = r_x, u_y = point->y - 2;
        while (u_x >= 0 && u_time < 6) {
            int result = 0;
            read_color(u_x, u_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(u_x + 1, u_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(u_x - 1, u_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(u_x + 2, u_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            read_color(u_x - 2, u_y, pixel);
            if (int(pixel[0]) != 3 && int(pixel[1]) != 168 && int(pixel[2]) != 245) {
                if (int(pixel[0]) != 229 && int(pixel[1] != 0 && pixel[2] != 0)) {
                    result++;
                }
            }
            if (result == 5) {
                fill_stack.push(new Point(u_x, u_y));
                break;
            }
            u_x -= 1;// 向右寻找可用点
            u_time++;
        }
    }
}
