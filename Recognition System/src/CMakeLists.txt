file(GLOB_RECURSE LIB_SRC *.cpp)
file(GLOB_RECURSE LIB_HEADERS *.h)


list(REMOVE_ITEM LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(tinyeye-core SHARED ${LIB_HEADERS} ${LIB_SRC})
add_executable(tinyeye-controller_module main.cpp)

find_package(OpenCV REQUIRED)
find_package(Caffe REQUIRED)
find_package(Torch REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(OpenSSL)

if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "please make sure libopencv-dev is installed")
endif()

if(NOT Caffe_FOUND)
    message(FATAL_ERROR "please make sure libcaffe is installed")
endif()

if (NOT Torch_FOUND)
    message(FATAL_ERROR "please make sure to set LIB_TORCH_PATH to the path of libtorch")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Caffe_INCLUDE_DIRS})
include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${SOCKETIO_INC})
include_directories(${CURL_INC})
include_directories(${OPENSSL_INCLUDE_DIR})

target_link_libraries(tinyeye-core ${OpenCV_LIBS} ${Caffe_LIBRARIES} ${TORCH_LIBRARIES} ${SOCKETIO_LIB} ${CURL_LIB} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
target_link_libraries(tinyeye-controller_module tinyeye-core)

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

install(TARGETS tinyeye-core
    EXPORT tinyeye-coreTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(TARGETS tinyeye-controller_module RUNTIME DESTINATION bin)
