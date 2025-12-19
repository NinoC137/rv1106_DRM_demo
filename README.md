# rv1106_DRM_demo

一个使用DRM驱动来驱动RGB888屏幕外设的最简demo

显示效果为屏幕渐变色



## 编译部署

```shell
gcc drm_test.c -o drm_test -I/usr/include/libdrm -ldrm
```



## 注意事项

1. 在使用交叉编译时，rv1106的工具链仅支持在x86架构上使用，如果宿主机是arm架构（比如我的mac mini M4），就会报错缺失动态库
