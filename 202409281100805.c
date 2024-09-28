// 帧和行长度设置
//将 LINE_LENGTH 设置为 1236 像素。
WriteAutoIncrement(0x0300, 0xa4, 0x06); // UI.STREAM_STATICS.LINE_LENGTH = 1700 //line time=10.5721393 us
// 将 OIF_CSI_BITRATE 设置为 600 Mbps。
WriteAutoIncrement(0x0312, 0x58, 0x02); // UI.STREAM_STATICS.OIF_CSI_BITRATE = 600
// FRAME_LENGTH 设置为 4336 行，对应 30 fps 的帧率。
WriteAutoIncrement(0x0458, 0x50, 0x0c); // UI.STREAM_CTX0.FRAME_LENGTH = 3152 // frame rate = 30 fps
WriteAutoIncrement(0x30c, 0x82, 0x00); // UI.STREAM_STATICS.OIF_CTRL // dual lane


// Exposure
/* 曝光设置 */
// 启用自动曝光（AE）
WriteAutoIncrement(0x044c, 0x00); // UI.STREAM_CTX0.EXPOSURE_MODE = 0 (AE on)
// AutoExposureTarget
// 设置自动曝光目标
WriteAutoIncrement(0x043c, 0x1e, 0x0); // (val=30) / UI.STREAM_DYNAMICS.AE_TARGET_PERCENTAGE.VALUE

// Resolution
/* 分辨率和输出区域设置 */
// 将 Y_START 设置为 182。
WriteAutoIncrement(0x045a, 0xb6, 0x00); // UI.STREAM_CTX0.Y_START = 182
// 将 Y_END 设置为 1181。
WriteAutoIncrement(0x045c, 0x9d, 0x04); // UI.STREAM_CTX0.Y_END = 1181
// ROI（感兴趣区域） 将 X_START 设置为 62。
WriteAutoIncrement(0x045e, 0x3e, 0x00); // UI.STREAM_CTX0.OUT_ROI_X_START = 62
// ROI（感兴趣区域） 将 X_END 设置为 1061。
WriteAutoIncrement(0x0460, 0x25, 0x04); // UI.STREAM_CTX0.OUT_ROI_X_END = 1061
// ROI（感兴趣区域） 将 Y_START 设置为 0。
WriteAutoIncrement(0x0462, 0x00, 0x00); // UI.STREAM_CTX0.OUT_ROI_Y_START = 0
// ROI（感兴趣区域） 将 Y_END 设置为 999。
WriteAutoIncrement(0x0464, 0xe7, 0x03); // UI.STREAM_CTX0.OUT_ROI_Y_END = 999

// Orientation
/* 图像方向和输出控制 */
// 设置图像方向
WriteAutoIncrement(0x0302, 0x2); // (val=2) / UI.STREAM_STATICS.ORIENTATION.MODE
// Output ctrl
// 配置输出控制
WriteAutoIncrement(0x0335, 0x1); // (val=1) / UI.STREAM_STATICS.OUTPUT_CTRL.CONTROL
