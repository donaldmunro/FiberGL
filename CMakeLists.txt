cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(FiberGL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)

set(DEPENDENCIES "${PROJECT_SOURCE_DIR}/libs")
set(USE_INSTALLED_GLFW FALSE)
set(USE_INSTALLED_GLM FALSE)
set(USE_GLAD FALSE)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package( Threads REQUIRED )
MESSAGE(STATUS "Threads Lib: " ${CMAKE_THREAD_LIBS_INIT})
MESSAGE(STATUS "Using PTHreads?: "  ${CMAKE_USE_PTHREADS_INIT})

if (USE_INSTALLED_GLFW)
   find_package(glfw3 3.2 REQUIRED)
else()
   set(GLFW_LOCAL "${DEPENDENCIES}/glfw")
   set(GLFW_LOCAL_GIT "${GLFW_LOCAL}/.git")
   if (EXISTS ${GLFW_LOCAL} AND EXISTS ${GLFW_LOCAL_GIT})
      execute_process(COMMAND git pull
            WORKING_DIRECTORY ${GLFW_LOCAL})
   else()
      execute_process(COMMAND git clone https://github.com/glfw/glfw
            WORKING_DIRECTORY ${DEPENDENCIES})
   endif()
   set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
   set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
   set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
   add_subdirectory(${GLFW_LOCAL})
   set(GLFW_INCLUDE_DIR "${GLFW_LOCAL}/include")
endif()

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
MESSAGE(STATUS "OpenGL include directory: " "${OPENGL_INCLUDE_DIR}")
MESSAGE(STATUS "OpenGL library: " "${OPENGL_LIBRARY}")
MESSAGE(STATUS "GLUT include directory: " "${GLUT_INCLUDE_DIR}")
MESSAGE(STATUS "GLUT library: " "${GLU_LIBRARY}")

if(USE_GLAD)
   set(GLAD_DIR "${DEPENDENCIES}/glad")
   set(GLAD_C "${GLAD_DIR}/src/glad.c")
   if (EXISTS ${GLAD_DIR} AND EXISTS ${GLAD_C})
      add_library("glad" ${GLAD_C})
      target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
   else()
      message( FATAL_ERROR "GLAD requested but not found (expected in ${GLAD_DIR}). To fix add ${GLAD_C} and ${GLAD_DIR}/include/glad.h or use GLEW by setting USE_GLAD to false." )
   endif()
else()
   find_package(GLEW REQUIRED)
   MESSAGE(STATUS "glew Include:" ${GLEW_INCLUDE_DIRS})
   MESSAGE(STATUS "glew library:" ${GLEW_LIBRARIES})
endif()

if(USE_INSTALLED_GLM)
   find_package(glm REQUIRED)
else()
   set(GLM_LOCAL "${DEPENDENCIES}/glm")
   set(GLM_LOCAL_GIT "${GLM_LOCAL}/.git")
   if (EXISTS ${GLM_LOCAL} AND EXISTS ${GLM_LOCAL_GIT})
      execute_process(COMMAND git pull
            WORKING_DIRECTORY ${GLM_LOCAL})
   else()
      execute_process(COMMAND git clone https://github.com/g-truc/glm
            WORKING_DIRECTORY ${DEPENDENCIES})
   endif()
   set(GLM_INCLUDE_DIRS "${GLM_LOCAL}/glm")
endif()

MESSAGE(STATUS "GLM include: " "${GLM_INCLUDE_DIRS}") # /ssd/src/c++/FiberGL/libs/glm/glm

find_package(Boost REQUIRED COMPONENTS context fiber)

message(STATUS "Boost Version:" "${Boost_VERSION}")
message(STATUS "Boost Include:" "${Boost_INCLUDE_DIRS}")

add_definitions( -DFILESYSTEM_EXPERIMENTAL )

add_executable(fibergl src/fibergl.cc src/OGLUtils.cc src/OGLUtils.h src/OGLFiberWin.cc src/OGLFiberWin.hh
                       src/tinyply.cpp src/tinyply.h/ src/Samples.cc src/Samples.h src/PointCloudWin.cc src/PointCloudWin.h)
if(USE_GLAD)
   target_compile_options( fibergl PRIVATE "-DFILESYSTEM_EXPERIMENTAL" "-DUSE_GLAD")
   target_include_directories(fibergl PUBLIC "${PROJECT_SOURCE_DIR}/src" "${GLAD_DIR}/include" "${GLM_INCLUDE_DIRS}")
else()
   target_compile_options( fibergl PRIVATE "-DFILESYSTEM_EXPERIMENTAL" "-DUSE_GLEW")
   target_include_directories(fibergl PUBLIC "${PROJECT_SOURCE_DIR}/src" "${GLFW_INCLUDE_DIR}" "${GLM_INCLUDE_DIRS}")
endif()
if (USE_INSTALLED_GLFW)
   if(USE_GLAD)
      target_link_libraries(fibergl z glfw glad ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)
   else()
      target_link_libraries(fibergl z glfw ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)
   endif()
else()
   target_include_directories(fibergl PUBLIC "${PROJECT_SOURCE_DIR}/src" "${GLM_INCLUDE_DIRS}" "${GLFW_INCLUDE_DIR}")
   if(USE_GLAD)
      target_link_libraries(fibergl z glfw glad ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)
   else()
      target_link_libraries(fibergl z glfw ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)
   endif()
endif()

