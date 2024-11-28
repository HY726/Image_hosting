#!/bin/bash

# 等待设备连接
adb wait-for-device

# 获取 root 权限
adb root
adb wait-for-device

# remount 文件系统为可写
adb remount

# 推送文件到设备
adb push camxoverridesettings.txt /vendor/etc/camera/

# 打印重启相机服务的消息
echo "---------restarting cameraserver-------------"

# 停止 camera.provider 服务进程
adb shell "kill \$(ps -A | grep camera.provider | awk '{print \$2}')"

# 模拟按下 Home 键
adb shell input keyevent 3

# 注释掉的命令：可以根据需要选择是否启用
# adb shell "for /f \"tokens=2\" %%a in ('\"adb shell ps -A | findstr mediaserver\"') do adb shell kill %%a"
# adb shell "for /f \"tokens=2\" %%a in ('\"adb shell ps -A | findstr mm-qcamera-daemon\"') do adb shell kill %%a"

# 停止 android.hardware.camera.provider@2.4-service_64 服务进程
for pid in $(adb shell ps -A | grep android.hardware.camera.provider@2.4-service_64 | awk '{print $2}'); do
    adb shell kill $pid
done

# 注释掉的命令：可以根据需要选择是否启用
# adb shell "for /f \"tokens=2\" %%a in ('\"adb shell ps -A | findstr camera\"') do adb shell kill %%a"

# 等待 5 秒
sleep 5

# 结束脚本
echo "Camera server restarted successfully"
