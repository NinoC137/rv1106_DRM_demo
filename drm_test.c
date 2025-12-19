#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_mode.h>

int main() {
    int fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) { perror("open"); return -1; }

    drmModeRes *res = drmModeGetResources(fd);
    drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[0]);
    drmModeModeInfo *mode = &conn->modes[0];

    struct drm_mode_create_dumb create = {0};
    create.width = mode->hdisplay;
    create.height = mode->vdisplay;
    create.bpp = 32;
    if (drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) { perror("CREATE_DUMB"); return -1; }

    struct drm_mode_map_dumb map = {0};
    map.handle = create.handle;
    if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map) < 0) { perror("MAP_DUMB"); return -1; }

    void *buf = mmap(NULL, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
    if (!buf) { perror("mmap"); return -1; }

    // 按行填充
    for (int y = 0; y < mode->vdisplay; y++) {
        uint32_t *row = (uint32_t *)((char*)buf + y * create.pitch);
        for (int x = 0; x < mode->hdisplay; x++) {
            row[x] = 0xFFFF0000; // 红色
        }
    }

    uint32_t fb_id;
    if (drmModeAddFB(fd, mode->hdisplay, mode->vdisplay, 24, 32, create.pitch, create.handle, &fb_id)) {
        perror("AddFB"); return -1;
    }

    // 直接用 legacy API 设置 CRTC
    drmModeSetCrtc(fd, res->crtcs[0], fb_id, 0, 0, &conn->connector_id, 1, mode);

    getchar();

    drmModeRmFB(fd, fb_id);
    munmap(buf, create.size);
    drmModeFreeConnector(conn);
    drmModeFreeResources(res);
    close(fd);

    return 0;
}

