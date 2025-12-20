cmake_minimum_required(VERSION 3.10)

project(drm_test C)

# 在 CMake 中为编译器添加 sysroot
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/opt/sysroot")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/opt/sysroot")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=/opt/sysroot")



# 生成可执行文件
add_executable(drm_test drm_test.c)

# 头文件路径，对应 -I/usr/include/libdrm
target_include_directories(drm_test PRIVATE
    /opt/sysroot/usr/include
    /opt/sysroot/usr/include/drm
)

# 链接 libdrm，对应 -ldrm
target_link_directories(drm_test PRIVATE 
	/opt/sysroot/lib
	/opt/sysroot/usr/lib/arm-linux-gnueabihf
	/opt/sysroot/usr/lib
	)
target_link_libraries(drm_test
    drm
)
