project(app)
cmake_minimum_required(VERSION 3.14.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(
        ${Boost_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/srv
)

file(GLOB SRC ./*.cpp)
file(GLOB HPP ./*.hpp)
add_executable(${PROJECT_NAME} ${SRC} ${HPP} )
target_link_libraries(${PROJECT_NAME} srv)
