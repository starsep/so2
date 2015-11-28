cmake_minimum_required(VERSION 2.8)
project(so2)

add_custom_target(so2 COMMAND make all -C ${so2_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})
