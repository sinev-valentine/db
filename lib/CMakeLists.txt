project(lib)
cmake_minimum_required(VERSION 3.14.2)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -DXMLSEC_CRYPTO_DYNAMIC_LOADING")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")

include_directories(
        ${Boost_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/libraries/fc_light/include
)

#file(GLOB SRC ./*.cpp)
file(GLOB HPP ./*.hpp)

add_library(${PROJECT_NAME}  ${HPP})
target_link_libraries(${PROJECT_NAME})
