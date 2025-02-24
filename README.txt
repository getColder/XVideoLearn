源码在以下环境编译与运行：

平台：windows11
Qt版本：5.15.2_msvc2019_32
预编译动态库：
* x264
* x265
* fdk-aac
* sdl2
* ffmpeg-4.3
动态库编译环境为: x86 Native Tools Command Prompt for VS 2022

XVideo
* 使用SDL手动渲染YUV，并封装SDL渲染YUV像素
* 加入复制、镜像等功能

XVideoAVFrame
* 使用AVFrame存储数据，使用SDL的YUV接口渲染。
* 加入线程帧率控制，使用自定义sleep函数增加准确度


XVideoMulti
* 增加XVideo多路复用界面
* XVideoView封装Open、Read接口读取YUV、RGB格式文件的读取
* 维护AVFrame对象成员存储读取数据