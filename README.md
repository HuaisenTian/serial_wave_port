# 串口调试助手

基于Qt5，支持实时串口数据绘制，最多可同时绘制8条数据曲线

![pic1.gif](https://github.com/tianhuaisen/serial_waveport/tree/main/pic/waveport.gif)

## 保存图片

![pic2.png](https://github.com/tianhuaisen/serial_waveport/tree/main/pic/2.png)

## 保存数据

![pic3.png](https://github.com/tianhuaisen/serial_waveport/tree/main/pic/1.png)

## 下位机发送数据格式

以绘制3条曲线为例，通过"\n"换行，x是数据1，y是数据2，z是数据3：

```c++
printf("%lf,%lf,%lf\n",x,y,z);
```

