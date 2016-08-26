project(Server_udp)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_FLAGS "--std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "-lmysqlclient -lpthread")
include_directories(credis) #增加include文件夹
aux_source_directory(. SRC_LIST)
aux_source_directory(credis SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
