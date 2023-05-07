#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
using namespace std;

int OpenGLInit();
void mouseHandler(GLFWwindow *window, int button, int action, int mods);
void mouseMoveHandler(GLFWwindow *window, double xpos, double ypos);
void draw_static();
GLFWwindow *window;
const int WIDTH = 1280;
const int HEIGHT = 960;

int domain_x1 = 0, domain_y1 = 0, domain_x2 = 0, domain_y2 = 0;// 领域范围
void domain_calculate();                                       // 领域计算

enum shapeType {
    LINE,
    ROUND,
    ELLIPSE,
    RECTANGLE,
    POLYGON,
};

shapeType currentType = LINE;
bool l_button_down = false;
bool r_button_down = false;

class Point {
public:
    int x;
    int y;
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

class Shape {
public:
    shapeType type;
    bool display;
    vector<Point *> *points;
    bool is_chosen;
    Shape(shapeType type) {
        this->type = type;
        this->display = false;
        this->is_chosen = false;
        points = new vector<Point *>();
    }
    ~Shape() {
        for (int i = 0; i < points->size(); i++) {
            delete points->at(i);
        }
        delete points;
    }
    void draw() {
        if (is_chosen) {
            // #FF5252
            glColor3f(1.0f, 0.32f, 0.32f);
        } else {
            // #2196F3
            glColor3f(0.129f, 0.588f, 0.953f);
        }
        switch (type) {
            case LINE:
                glBegin(GL_LINES);
                glVertex2i(points->at(0)->x, points->at(0)->y);
                glVertex2i(points->at(1)->x, points->at(1)->y);
                glEnd();
                break;
            case ROUND:
                break;
            case ELLIPSE:
                break;
            case RECTANGLE:
                break;
            case POLYGON:
                break;
            default:
                break;
        }
    }
    bool isChosen() {
        int x_min = domain_x1 > domain_x2 ? domain_x2 : domain_x1;
        int x_max = domain_x1 > domain_x2 ? domain_x1 : domain_x2;
        int y_min = domain_y1 > domain_y2 ? domain_y2 : domain_y1;
        int y_max = domain_y1 > domain_y2 ? domain_y1 : domain_y2;
        switch (type) {
            case LINE: {
                // 判断两个端点是否在领域内
                int x1 = points->at(0)->x;
                int y1 = points->at(0)->y;
                int x2 = points->at(1)->x;
                int y2 = points->at(1)->y;
                if (x1 >= x_min && x1 <= x_max && y1 >= y_min && y1 <= y_max &&
                    x2 >= x_min && x2 <= x_max && y2 >= y_min && y2 <= y_max) {
                    return true;
                }
            } break;
            case ROUND:
                break;
            case ELLIPSE:
                break;
            case RECTANGLE:
                break;
            case POLYGON:
                break;
            default:
                break;
        }
        return false;
    }
};

vector<Shape *> *shapes = new vector<Shape *>();

int main() {
    OpenGLInit();
    glfwSetMouseButtonCallback(window, mouseHandler);
    glfwSetCursorPosCallback(window, mouseMoveHandler);
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
    window = glfwCreateWindow(WIDTH, HEIGHT, "变换", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    // 设置坐标系
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glClearColor(1, 1, 1, 1);
    glLineWidth(5.0);
    return 0;
}

void mouseHandler(GLFWwindow *window, int button, int action, int mods) {
    glClear(GL_COLOR_BUFFER_BIT);
    double x, y;
    glfwGetCursorPos(window, &x, &y);// 这里拿到的位置 始终以左上角为原点 不受 glOrtho的影响
    int x1 = int(x);
    int y1 = int(HEIGHT - y);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        l_button_down = true;
        cout << "起始点" << x1 << " " << y1 << endl;
        Shape *currentShape = new Shape(currentType);
        currentShape->points->push_back(new Point(x1, y1));
        shapes->push_back(currentShape);
        return;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        l_button_down = false;
        cout << "终结" << x1 << " " << y1 << endl;
        auto currentShape = shapes->back();
        currentShape->points->push_back(new Point(x1, y1));
        currentShape->display = true;// 左键抬起 常驻显示
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        r_button_down = true;
        cout << "右键按下" << x1 << " " << y1 << endl;
        domain_x1 = x1;
        domain_y1 = y1;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        r_button_down = false;
        cout << "右键抬起" << x1 << " " << y1 << endl;
        domain_x2 = x1;
        domain_y2 = y1;
        domain_calculate();
    }
    draw_static();
    glfwSwapBuffers(window);
}


void draw_static() {
    // 在这里负责把所有需要显示的图像绘制出来 根据Shape类
    for (auto shape: *shapes) {
        if (shape->display) {
            shape->draw();
        }
    }
    // 什么时候交换合适呢？为什么不在draw里交换
}
void domain_calculate() {
    // 计算领域
    // 全置为false
    for (auto shape: *shapes) {
        shape->is_chosen = false;
    }
    // 遍历所有图形
    for (auto shape: *shapes) {
        if (shape->isChosen()) {
            shape->is_chosen = true;
            return;
        }
    }
}

void mouseMoveHandler(GLFWwindow *window, double xpos, double ypos) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (r_button_down) {
        // 右键按下
        int x2 = int(xpos);
        int y2 = int(HEIGHT - ypos);
        domain_x2 = x2;
        domain_y2 = y2;
        draw_static();
        // 画出领域
        // #E91E63
        glColor3f(0.9137, 0.1176, 0.3882);
        // 画出矩形
        glBegin(GL_LINE_LOOP);
        glVertex2i(domain_x1, domain_y1);
        glVertex2i(domain_x1, domain_y2);
        glVertex2i(domain_x2, domain_y2);
        glVertex2i(domain_x2, domain_y1);
        glEnd();
        glfwSwapBuffers(window);
    }
}