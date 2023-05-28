cmake_minimum_required(VERSION 3.24)
project(opengl)

set(CMAKE_CXX_STANDARD 17)

set(Class 3) # 5. 图形变换 4. 图形填充  3. 裁剪 2. 画圆 1. 初始化

IF (WIN32)
    include_directories("D:\\Env\\GLFW\\include")
    include_directories("D:\\Env\\Eigen")
    include_directories("D:\\Env\\FreeGLUT\\include")
    link_directories("D:\\Env\\GLFW\\lib")
    link_directories("D:\\Env\\FreeGLUT\\lib\\Debug")

ELSEIF (APPLE)
    include_directories(/Users/xwwwb/Coding/my_script/glfw/include)
    link_directories(/Users/xwwwb/Coding/my_script/glfw/lib)
    include_directories(/Users/xwwwb/Coding/my_script/eigen-3.4.0)
ENDIF ()

IF (Class EQUAL 1)
    add_executable(opengl src/init_opengl/main.cpp)
ELSEIF (Class EQUAL 2)
    add_executable(opengl src/draw_circle/main.cpp)
ELSEif (Class EQUAL 3)
    add_executable(opengl src/cropping/main.cpp)
ELSEIF (Class EQUAL 4)
    add_executable(opengl src/graphic_fill/main.cpp)
ELSEIF (Class EQUAL 5)
    add_executable(opengl src/transform/main.cpp)
ENDIF ()

IF (WIN32)
    target_link_libraries(opengl glfw3 freeglutd opengl32)
ELSEIF (APPLE)
    target_link_libraries(opengl glfw3 "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo")
ENDIF ()