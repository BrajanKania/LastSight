cmake_minimum_required(VERSION 3.23)

add_library(shader)

target_sources(shader
  PRIVATE
    shader.cpp
    shader_exception.cpp

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    FILES
      ../shader.hpp

    PRIVATE
    FILE_SET implementation_headers
    TYPE HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
      shader_exception.hpp
)

target_link_libraries(shader 
  PUBLIC 
    glm::glm
    glad2_lib
)
