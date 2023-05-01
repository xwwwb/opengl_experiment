cmake_minimum_required(VERSION 3.24)
project(opengl)

set(CMAKE_CXX_STANDARD 17)

IF (WIN32)
    # 头文件引入
    include_directories("D:\\Env\\GLFW\\include")
    # 库文件目录
    link_directories("D:\\Env\\GLFW\\lib")
    add_executable(opengl main.cpp)
    target_link_libraries(opengl glfw3 opengl32)
ELSEIF (APPLE)
    # 头文件引入
    include_directories(/Users/xwwwb/Coding/my_script/glfw/include)
    # 库文件目录
    link_directories(/Users/xwwwb/Coding/my_script/glfw/lib)
    add_executable(opengl main.cpp)
    target_link_libraries(opengl glfw3 "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo")
ENDIF ()

