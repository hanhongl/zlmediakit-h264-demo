cmake_minimum_required(VERSION 3.1.3)

project(publish_h264)

add_executable(${PROJECT_NAME} push.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(${PROJECT_NAME} mk_api)

