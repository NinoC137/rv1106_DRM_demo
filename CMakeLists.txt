cmake_minimum_required(VERSION 3.10)

project(drm_test C)

# 生成可执行文件
add_executable(drm_test drm_test.c)

# 头文件路径，对应 -I/usr/include/libdrm
target_include_directories(drm_test PRIVATE
    /usr/include/libdrm
)

# 链接 libdrm，对应 -ldrm
target_link_libraries(drm_test
    drm
)
