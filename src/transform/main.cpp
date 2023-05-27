#include <Eigen/Dense>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

int OpenGLInit();
void mouseHandler(GLFWwindow *window, int button, int action, int mods);
void mouseMoveHandler(GLFWwindow *window, double xpos, double ypos);
void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
void draw_static();
void domain_calculate();// 领域计算
void calculate();       // 进行变换

GLFWwindow *window;
const int WIDTH = 1280;
const int HEIGHT = 960;
const double PI = 3.1415926;
int domain_x1 = 0, domain_y1 = 0, domain_x2 = 0, domain_y2 = 0;// 领域范围

enum shapeType {
    LINE,
    RECTANGLE,
    TRIANGLE,
    ROUND,
};
enum OPERATE {
    MOVE = 1,
    ROTATE,
    SCALE,
    SHEAR,
    DEFAULT,
};

vector<string> text = {"直线", "矩形", "三角形", "圆"};
vector<string> op_text = {"位移", "旋转", "缩放", "错切", "默认"};

int currentType = RECTANGLE;
OPERATE currentOperate = DEFAULT;
bool l_button_down = false;
bool r_button_down = false;
bool calculate_result = false;

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
    static Shape *chosen_one;// 静态成员 被选中的那个对象
    vector<Point *> *others;
    Shape(shapeType type) {
        this->type = type;
        this->display = false;
        points = new vector<Point *>();
        others = new vector<Point *>();
    }
    ~Shape() {
        for (int i = 0; i < points->size(); i++) {
            delete points->at(i);
        }
        delete points;
    }
    void draw() {
        if (this == chosen_one) {
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
            case RECTANGLE:
                glBegin(GL_LINE_LOOP);
                glVertex2i(points->at(0)->x, points->at(0)->y);
                glVertex2i(points->at(1)->x, points->at(1)->y);
                glVertex2i(points->at(2)->x, points->at(2)->y);
                glVertex2i(points->at(3)->x, points->at(3)->y);
                glEnd();
                break;
            case TRIANGLE:
                glBegin(GL_LINE_LOOP);
                glVertex2i(points->at(0)->x, points->at(0)->y);
                glVertex2i(points->at(1)->x, points->at(0)->y);
                glVertex2i(points->at(2)->x, points->at(2)->y);
                glEnd();
                break;
            case ROUND: {
                glBegin(GL_POINTS);
                for (int i = 0; i < points->size(); i++) {
                    glVertex2i(points->at(i)->x, points->at(i)->y);
                }
                glEnd();
                break;
            }
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
                break;
            }
            case RECTANGLE: {
                // 判断两个端点是否在领域内
                int x1 = points->at(0)->x;
                int y1 = points->at(0)->y;
                int x2 = points->at(2)->x;
                int y2 = points->at(2)->y;
                if (x1 >= x_min && x1 <= x_max && y1 >= y_min && y1 <= y_max &&
                    x2 >= x_min && x2 <= x_max && y2 >= y_min && y2 <= y_max) {
                    return true;
                }
                break;
            }
            case TRIANGLE: {
                int x1 = points->at(0)->x;
                int y1 = points->at(0)->y;
                int x2 = points->at(1)->x;
                int y2 = points->at(2)->y;
                if (x1 >= x_min && x1 <= x_max && y1 >= y_min && y1 <= y_max &&
                    x2 >= x_min && x2 <= x_max && y2 >= y_min && y2 <= y_max) {
                    return true;
                }
                break;
            }
            case ROUND: {
                // 这里的other 可不会随着变换而变换 要一直以原图的大小和位置进行框选
                int x = others->at(0)->x;
                int y = others->at(0)->y;
                int r = others->at(1)->x;
                if (x - r >= x_min && x + r <= x_max && y - r >= y_min && y + r <= y_max) {
                    return true;
                }
                break;
            }
            default:
                break;
        }
        return false;
    }
    void transform(Eigen::Matrix3d *matrix) {
        int dx, dy;
        // 直线按直线中点做中心点 错切除外
        if (type == LINE && currentOperate != SHEAR) {
            dx = (points->front()->x + points->back()->x) / 2;
            dy = (points->front()->y + points->back()->y) / 2;
        } else if (type == LINE && currentOperate == SHEAR) {
            // 选择直线中离原点近的
            dx = points->front()->x > points->back()->x ? points->back()->x : points->front()->x;
            dy = points->front()->y > points->back()->y ? points->back()->y : points->front()->y;
        } else if (type == TRIANGLE) {
            // 按三角形重心做中心点
            dx = (points->at(0)->x + points->at(1)->x + points->at(2)->x) / 3;
            dy = (points->at(0)->y + points->at(1)->y + points->at(2)->y) / 3;
        } else if (type == RECTANGLE) {
            dx = (points->at(0)->x + points->at(2)->x) / 2;
            dy = (points->at(0)->y + points->at(2)->y) / 2;
        } else if (type == ROUND) {
            dx = others->at(0)->x;
            dy = others->at(0)->y;
        } else {
            // 默认按第一个点做中心点
            dx = points->front()->x;
            dy = points->front()->y;
        }
        for (auto point: *points) {
            // 根据中心点位移
            point->x -= dx;
            point->y -= dy;
        }
        // 构造齐次矩阵
        // 下面这行有问题
        auto *point_matrix = new Eigen::Matrix<double, Eigen::Dynamic, 3>();
        point_matrix->resize(points->size(), 3);
        for (int i = 0; i < points->size(); i++) {
            (*point_matrix)(i, 0) = points->at(i)->x;
            (*point_matrix)(i, 1) = points->at(i)->y;
            (*point_matrix)(i, 2) = 1;
        }
        // 进行变换
        *point_matrix = *point_matrix * (*matrix);
        // 更新点的坐标
        for (int i = 0; i < points->size(); i++) {
            points->at(i)->x = int((*point_matrix)(i, 0));
            points->at(i)->y = int((*point_matrix)(i, 1));
        }
        // 位移回
        for (auto point: *points) {
            point->x += dx;
            point->y += dy;
        }
    }
};

Shape *Shape::chosen_one = nullptr;
vector<Shape *> *shapes;

int main() {
    OpenGLInit();
    shapes = new vector<Shape *>();
    glfwSetMouseButtonCallback(window, mouseHandler);
    glfwSetCursorPosCallback(window, mouseMoveHandler);
    glfwSetKeyCallback(window, keyHandler);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    while (!glfwWindowShouldClose(window)) {
        if (calculate_result) {
            calculate();
        }
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
    glPointSize(5.0);
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
        auto *currentShape = new Shape(shapeType(currentType));
        if (currentType == ROUND) {
            currentShape->others->push_back(new Point(x1, y1));
        } else {
            currentShape->points->push_back(new Point(x1, y1));
        }
        shapes->push_back(currentShape);
        return;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        l_button_down = false;
        auto currentShape = shapes->back();
        if (currentType == LINE) {
            currentShape->points->push_back(new Point(x1, y1));
        } else if (currentType == TRIANGLE) {
            currentShape->points->push_back(new Point(x1, currentShape->points->front()->y));
            // 三角形的第三个点是根据第一个点的x坐标和第二个点的y坐标确定的 等边三角形
            double x_1 = currentShape->points->front()->x;
            double y_1 = currentShape->points->front()->y;
            double y_2 = currentShape->points->back()->y;
            double x_2 = currentShape->points->back()->x;
            double x_3 = abs(x_2 + x_1) / 2;
            double y_3 = y_1 + sqrt(3) / 2 * abs(x_2 - x_1);
            currentShape->points->push_back(new Point(int(x_3), int(y_3)));
        } else if (currentType == RECTANGLE) {
            currentShape->points->push_back(new Point(x1, currentShape->points->front()->y));
            currentShape->points->push_back(new Point(x1, y1));
            currentShape->points->push_back(new Point(currentShape->points->front()->x, y1));
        } else if (currentType == ROUND) {
            int circle_x = currentShape->others->front()->x;
            int circle_y = currentShape->others->front()->y;
            int r = int(sqrt(pow(x1 - circle_x, 2) + pow(y1 - circle_y, 2)));
            currentShape->others->push_back(new Point(r, 0));// 圆的前俩点一个是中点 一个是r
            int x = 0;
            int y = r;
            int d = 1 - r;
            while (x < y) {
                // 画出八个点
                currentShape->points->push_back(new Point(x + circle_x, y + circle_y));
                currentShape->points->push_back(new Point(y + circle_x, x + circle_y));
                currentShape->points->push_back(new Point(-x + circle_x, y + circle_y));
                currentShape->points->push_back(new Point(-y + circle_x, x + circle_y));
                currentShape->points->push_back(new Point(x + circle_x, -y + circle_y));
                currentShape->points->push_back(new Point(y + circle_x, -x + circle_y));
                currentShape->points->push_back(new Point(-x + circle_x, -y + circle_y));
                currentShape->points->push_back(new Point(-y + circle_x, -x + circle_y));
                if (d < 0) {
                    d = d + 2 * x + 3;
                } else {
                    d = d + 2 * (x - y) + 5;
                    y = y - 1;
                }
                x = x + 1;
            }
        }

        currentShape->display = true;// 左键抬起 常驻显示
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        r_button_down = true;

        domain_x1 = x1;
        domain_y1 = y1;
        // 将chosen_one置空
        Shape::chosen_one = nullptr;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        r_button_down = false;

        domain_x2 = x1;
        domain_y2 = y1;
        domain_calculate();
        calculate_result = true;
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
    // 遍历所有图形
    for (auto shape: *shapes) {
        if (shape->isChosen()) {
            Shape::chosen_one = shape;
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

// 在这里构造变换矩阵
void calculate() {
    if (Shape::chosen_one == nullptr) {
        return;
    }
    using namespace Eigen;
    cout << "当前选中图像：" << text[Shape::chosen_one->type] << "\t请选择操作：" << endl;
    for (int i = MOVE; i < DEFAULT; i++) {
        cout << i << ". " << op_text[i - 1] << "\t";
    }
    cout << endl;
    int result;
    cin >> result;
    if (result < MOVE || result > DEFAULT) {
        cout << "输入错误" << endl;
        return;
    }
    currentOperate = OPERATE(result);
    switch (result) {
        case MOVE: {
            cout << "请输入移动的x值和y值：" << endl;
            int x, y;
            cin >> x >> y;
            // 构造变换矩阵
            auto *matrix = new Matrix<double, 3, 3>();
            *matrix << 1, 0, 0,
                    0, 1, 0,
                    x, y, 1;
            cout << "变换矩阵：" << endl;
            cout << *matrix << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            Shape::chosen_one->transform(matrix);
            delete matrix;
            break;
        };
        case ROTATE: {
            cout << "请输入旋转的角度：" << endl;
            double angle;// 度数
            cin >> angle;
            angle = angle * PI / 180;
            // 构造变换矩阵
            auto *matrix = new Matrix<double, 3, 3>();
            *matrix << cos(angle), sin(angle), 0,
                    -sin(angle), cos(angle), 0,
                    0, 0, 1;
            cout << "变换矩阵：" << endl;
            cout << *matrix << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            Shape::chosen_one->transform(matrix);
            delete matrix;
            break;
        }
        case SCALE: {
            cout << "请输入缩放的倍数：" << endl;
            double scale;
            cin >> scale;
            // 构造变换矩阵
            auto *matrix = new Matrix<double, 3, 3>();
            *matrix << scale, 0, 0,
                    0, scale, 0,
                    0, 0, 1;
            cout << "变换矩阵：" << endl;
            cout << *matrix << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            Shape::chosen_one->transform(matrix);
            delete matrix;
            break;
        }
        case SHEAR: {
            cout << "请分别输入X方向和Y方向的错切角度：" << endl;
            double x, y;
            cin >> x >> y;
            x = x * PI / 180;
            y = y * PI / 180;
            // 构造变换矩阵
            auto *matrix = new Matrix<double, 3, 3>();
            *matrix << 1, tan(y), 0,
                    tan(x), 1, 0,
                    0, 0, 1;
            cout << "变换矩阵：" << endl;
            cout << *matrix << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            Shape::chosen_one->transform(matrix);
            delete matrix;
            break;
        }
        default: {
        }
    }
    draw_static();
    glfwSwapBuffers(window);
    calculate_result = false;
}

void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        // 更改currentType
        currentType = (currentType + 1) % 4;
        cout << "当前图形：" << text[currentType] << endl;
    }
}
