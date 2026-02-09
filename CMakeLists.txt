cmake_minimum_required(VERSION 3.10)
project(resi_yolo_gss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(resi_yolo
    src/main.c
    src/gss_runner.c
    src/yolo_label.c
    src/utils.c
)
