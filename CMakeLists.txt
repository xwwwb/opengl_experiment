cmake_minimum_required(VERSION 3.24)
project(opengl)

set(CMAKE_CXX_STANDARD 17)

set(Class 5) # 5. 图形变换 4. 图形填充

IF (WIN32)
    include_directories("D:\\Env\\GLFW\\include")
    include_directories("D:\\Env\\Eigen")
    link_directories("D:\\Env\\GLFW\\lib")

ELSEIF (APPLE)
    include_directories(/Users/xwwwb/Coding/my_script/glfw/include)
    link_directories(/Users/xwwwb/Coding/my_script/glfw/lib)
    include_directories(/Users/xwwwb/Coding/my_script/eigen-3.4.0)
ENDIF ()

IF (Class EQUAL 4)
    add_executable(opengl src/graphic_fill/main.cpp)
ELSEIF (Class EQUAL 5)
    add_executable(opengl src/transform/main.cpp)
ENDIF ()


IF (WIN32)
    target_link_libraries(opengl glfw3 opengl32)
ELSEIF (APPLE)
    target_link_libraries(opengl glfw3 "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo")
ENDIF ()