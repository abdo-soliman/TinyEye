file(GLOB_RECURSE LIB_SRC *.cpp)
file(GLOB_RECURSE LIB_HEADERS *.h)


list(REMOVE_ITEM LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(tinyeye-core ${LIB_SRC} ${LIB_HEADERS})
add_executable(tinyeye-controller_module main.cpp)

find_package(OpenCV REQUIRED)

if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "please make sure libopencv-dev is installed")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(tinyeye-core ${OpenCV_LIBS})

if (USE_TENSORFLOW)
    include_directories(${TFLITE_INC})
elseif (USE_TORCH)
    find_package(Torch REQUIRED)
    if (NOT Torch_FOUND)
        message(FATAL_ERROR "please make sure to set LIB_TORCH_PATH to the path of libtorch")
    endif()
    include_directories(${TORCH_INCLUDE_DIRS})
endif()

if (USE_TENSORFLOW)
    target_link_libraries(tinyeye-controller_module tinyeye-core "${TFLITE_LIB}")
elseif (USE_PYTORCH)
    target_link_libraries(tinyeye-controller_module tinyeye-core "${TORCH_LIBRARIES}")
endif()
