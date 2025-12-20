# rv1106_DRM_demo

一个使用DRM驱动来驱动RGB888屏幕外设的最简demo

显示效果为屏幕渐变色

## 使用方法

### 同步目标机的库与头文件目录

```bash
sudo rsync -av -e "ssh -p port" root@target:/lib /opt/sysroot/
sudo rsync -av -e "ssh -p port" root@target:/usr/lib /opt/sysroot/usr/
sudo rsync -av -e "ssh -p port" root@target:/usr/include /opt/sysroot/usr/
```

然后编译部署docker，并运行

```bash
docker build -t drm-cross-env .
docker run -it --name drm-dev\
  -v $(pwd):/work \
  -v /opt/sysroot:/opt/sysroot:ro \
  drm-cross-env
```

## 编译部署

```shell
mkdir build
cd build
cmake ..
make
```



## 注意事项

1. 要确保库与头文件目录有正确地同步过来

2. 确保CMakeLists中有正确指定sysroot等参数