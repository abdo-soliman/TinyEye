file(GLOB_RECURSE LIB_SRC *.cpp)
file(GLOB_RECURSE LIB_HEADERS *.h)

list(REMOVE_ITEM LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(tinyeye-server SHARED ${LIB_HEADERS} ${LIB_SRC})
add_executable(tinyeye-server_module main.cpp)

find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
find_package(Caffe REQUIRED)

if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "please make sure libopencv-dev is installed")
endif()

if (NOT Torch_FOUND)
    message(FATAL_ERROR "please make sure to set LIB_TORCH_PATH to the path of libtorch")
endif()

if(NOT Caffe_FOUND)
    message(FATAL_ERROR "please make sure libcaffe is installed")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Caffe_INCLUDE_DIRS})
include_directories(${TORCH_INCLUDE_DIRS})

target_link_libraries(tinyeye-server ${OpenCV_LIBS} ${Caffe_LIBRARIES} ${TORCH_LIBRARIES})
target_link_libraries(tinyeye-server_module tinyeye-server)

function(install_dir src_dir_path destination_name)
    file(GLOB DEPLOY_FILES_AND_DIRS "${src_dir_path}/*")
    foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
        if (IS_DIRECTORY "${ITEM}")
            list(APPEND DIRS_TO_DEPLOY "${ITEM}")
        else()
            list(APPEND FILES_TO_DEPLOY "${ITEM}")
        endif()
    endforeach()

    install(FILES ${FILES_TO_DEPLOY} DESTINATION ${destination_name})
    install(DIRECTORY ${DIRS_TO_DEPLOY} DESTINATION ${destination_name})
endfunction()

install_dir("${CMAKE_SOURCE_DIR}/include" "include")
install_dir("${CMAKE_SOURCE_DIR}/models" "models")

install(TARGETS tinyeye-server
    EXPORT tinyeye-serverTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(TARGETS tinyeye-server_module RUNTIME DESTINATION bin)
