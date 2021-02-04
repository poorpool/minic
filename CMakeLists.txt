cmake_minimum_required(VERSION 3.17)
project(minic C)

set(CMAKE_C_STANDARD 11)

add_executable(codes main.c format.c scan.c print.c expression.c show.c show.h) # 这儿可能要加 .h

# 加上这一句，在读/撤销读 token 时显示输出
# add_definitions("-DSHOW_TOKEN")

# 加上这一句，显示抽象语法树
add_definitions("-DSHOW_AST")