# UAV-Circle-Detection

实现圆心检测和连线

## 编译方法

- windows
将根目录下`CMakeLists.txt`文件中的`$ENV{OPENCV_2413_PREBUILD_DIR}`改为自己OpenCV的`build`文件夹路径。

- Ubuntu
安装好OpenCV之后，可以执行`sh run.sh`直接编译运行

## 文件说明
- `MST` 文件夹为最小生成树相关文件
- `source` 文件夹为圆形检测相关文件