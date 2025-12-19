#include <stdio.h>      // 标准输入输出库，使用 perror, printf 等
#include <stdlib.h>     // 标准库，使用 exit, malloc, free 等
#include <fcntl.h>      // 文件控制，使用 open
#include <unistd.h>     // UNIX 标准函数，如 close
#include <sys/mman.h>   // 内存映射，使用 mmap / munmap
#include <stdint.h>     // 固定宽度整数类型，如 uint32_t

#include <time.h>

#include <xf86drm.h>        // libdrm 核心 API，包括 drmIoctl、drmModeAddFB 等
#include <xf86drmMode.h>    // libdrm KMS API，管理 connector、CRTC、encoder、framebuffer
#include <drm/drm_mode.h>   // DRM ioctl 常量定义，如 DRM_IOCTL_MODE_CREATE_DUMB / MAP_DUMB

int main() {
    // -----------------------------
    // 1. 打开 DRM 设备
    // /dev/dri/card0 是 DRM KMS 控制器设备
    // O_RDWR 表示可读写
    int fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) { perror("open"); return -1; }

    // -----------------------------
    // 2. 获取 DRM 资源
    // drmModeGetResources 返回 drmModeRes 结构体，包含：
    //  - 所有 CRTC 列表
    //  - 所有 connector 列表
    //  - 所有 encoder 列表
    drmModeRes *res = drmModeGetResources(fd);

    // 3. 获取第一个 connector（显示输出端口）
    // drmModeConnector 包含：
    //  - 连接状态 (connection)
    //  - 支持的显示模式 (modes)
    //  - encoder_id 关联的编码器
    drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[0]);

    // 4. 选择第一个显示模式
    // drmModeModeInfo 包含：
    //  - hdisplay / vdisplay 分辨率
    //  - 频率刷新率
    //  - timing 参数
    drmModeModeInfo *mode = &conn->modes[0];

    // -----------------------------
    // 5. 创建 dumb buffer（CPU 可访问的 framebuffer）
    // struct drm_mode_create_dumb 包含：
    //  - width / height / bpp
    //  - handle: buffer 内核句柄（输出）
    struct drm_mode_create_dumb create = {0};
    create.width = mode->hdisplay;
    create.height = mode->vdisplay;
    create.bpp = 32; // 32 bit per pixel (ARGB8888)
    if (drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) { perror("CREATE_DUMB"); return -1; }

    // -----------------------------
    // 6. 映射 dumb buffer 到用户空间
    struct drm_mode_map_dumb map = {0};
    map.handle = create.handle;    // 内核 buffer handle
    if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map) < 0) { perror("MAP_DUMB"); return -1; }

    // mmap 返回用户态虚拟地址
    void *buf = mmap(NULL, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
    if (!buf) { perror("mmap"); return -1; }

    // -----------------------------
    // 7. 将 dumb buffer 注册为 framebuffer
    // drmModeAddFB 会返回 framebuffer ID
    // 参数：
    //  - width / height
    //  - depth / bpp
    //  - pitch 每行字节数
    //  - handle 内核 buffer handle
    //  - fb_id 输出 framebuffer ID
    uint32_t fb_id;
    if (drmModeAddFB(fd, mode->hdisplay, mode->vdisplay, 24, 32, create.pitch, create.handle, &fb_id)) {
        perror("AddFB"); return -1;
    }
    
    // -----------------------------
    // 8. 设置 framebuffer 输出到 CRTC（显示控制器）
    // drmModeSetCrtc 参数：
    //  - fd DRM 设备
    //  - crtc_id 使用资源列表的第一个 CRTC
    //  - fb_id framebuffer ID
    //  - x, y framebuffer 偏移
    //  - connectors array 关联的 connector
    //  - count connectors 数量
    //  - mode 指定显示时序
    drmModeSetCrtc(fd, res->crtcs[0], fb_id, 0, 0, &conn->connector_id, 1, mode);
 
    // -----------------------------
    // 9. 按行填充 framebuffer 内容
    // 使用 create.pitch（每行字节数）计算每行起始地址
    int color_count = 256;
    while (1) {
        for (int c = 0; c < color_count; c++) {
            uint32_t color = 
                0xff000000 |
                (c << 16) |
                ((255 - c) << 8) |
                c;

            for (int y = 0; y < mode->vdisplay; y++) {
                uint32_t *row =
                    (uint32_t *)((char*)buf + y * create.pitch);
                for (int x = 0; x < mode->hdisplay; x++) {
                    row[x] = color;
                }
            }
            //usleep(500 * 1000); // 500ms
        }

        if(getchar()){
            break;
        }
    }    

    // -----------------------------
    // 11. 清理资源
    drmModeRmFB(fd, fb_id);          // 移除 framebuffer
    munmap(buf, create.size);        // 解除内存映射
    drmModeFreeConnector(conn);      // 释放 connector
    drmModeFreeResources(res);       // 释放资源
    close(fd);                       // 关闭 DRM 设备

    return 0;
}
