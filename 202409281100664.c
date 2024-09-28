/* 调整时钟 */
// Frequency
// 频率和时钟设置
WriteAutoIncrement(0x0220, 0x00, 0x36, 0x6e, 0x01); // UI.SENSOR_SETTINGS.EXT_CLOCK = 24000000
// 配置 PLL 和时钟分频器
WriteAutoIncrement(0x0224, 0x04); // UI.SENSOR_SETTINGS.CLK_PLL_PREDIV = 4
WriteAutoIncrement(0x0225, 0x1); // (val=1) / UI.SENSOR_SETTINGS.CLK_PLL_POSTDIV.VALUE
WriteAutoIncrement(0x0226, 0x86); // UI.SENSOR_SETTINGS.CLK_SYS_PLL_MULT  = 134
WriteAutoIncrement(0x0227, 0x05); // UI.SENSOR_SETTINGS.VT_CLK_DIV = 5

// Format
/* 格式设置 */
// 设置输出格式,将 OUT_FORMAT 设置为 10-bit。
WriteAutoIncrement(0x030a, 0xa); // (val=10) / UI.STREAM_STATICS.FORMAT_CTRL.OUT_FORMAT //10-bit
// 设置 CSI 接口的数据类型,设置 DATA_TYPE，与 MIPI CSI-2 规范匹配。
WriteAutoIncrement(0x030f, 0x2b); // (val=43) / UI.STREAM_STATICS.OIF_IMG_CTRL.DATA_TYPE
// Correction
/* 校正设置 */
// 设置 DUSTER_DMP_CC_SIGMA_Y 的低阈值和高阈值为 8。
WriteAutoIncrement(0x031c, 0x0a, 0x00, 0x50, 0x00); // UI.STREAM_STATICS.DUSTER_DMP_CC_SIGMA_X
WriteAutoIncrement(0x0320, 0x08, 0x00, 0x08, 0x00); // UI.STREAM_STATICS.DUSTER_DMP_CC_SIGMA_Y
// DarkCal
// 将 DARKCAL_CTRL.MODE 设置为 1，启用暗电流校准。
// WriteAutoIncrement(0x0340, 0x1); // (val=1) / UI.STREAM_STATICS.DARKCAL_CTRL.MODE
// DarkLines
// 禁用暗行输出
WriteByte(0x332, 0x00); // UI.STREAM_STATICS.OUTPUT_DARK_ROWS
WriteByte(0x414, 0x00); // UI.STREAM_DYNAMICS.DARKCAL_CTRL =   Enable Darkcal in automatic mode
// Pedestal
//设置暗电流校准基准
WriteAutoIncrement(0x0415, 0x40); // (val=64) / UI.STREAM_DYNAMICS.DARKCAL_PEDESTAL.VALUE
WriteAutoIncrement(0x043e, 0x8c, 0x0); // (val=140) / UI.STREAM_DYNAMICS.AE_STEP_PROPORTION.VALUE
WriteAutoIncrement(0x0440, 0xcc, 0x2c); // (val=11468) / UI.STREAM_DYNAMICS.AE_LEAK_PROPORTION.VALUE

// GPIO
/* 配置 GPIO 模式 */
// GPIO_0_header
WriteAutoIncrement(0x0467, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_0_CTRL.Mode
// GPIO_1_header
WriteAutoIncrement(0x0468, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_1_CTRL.Mode
// GPIO_2_header
WriteAutoIncrement(0x0469, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_2_CTRL.Mode
// GPIO_3_header
WriteAutoIncrement(0x046a, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_3_CTRL.Mode
// GPIO_4_header
WriteAutoIncrement(0x046b, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_4_CTRL.Mode
// GPIO_5_header
WriteAutoIncrement(0x046c, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_5_CTRL.Mode
// GPIO_6_led
WriteAutoIncrement(0x046d, 0x2); // (val=2) / UI.STREAM_CTX0.GPIO_6_CTRL.Mode
// GPIO_7_spare
WriteAutoIncrement(0x046e, 0x5); // (val=5) / UI.STREAM_CTX0.GPIO_7_CTRL.Mode
// Strobe_StartDelay
// 配置闪光灯启动延迟
WriteByte(0x0471, 0x00); // (val=0) / UI.STREAM_CTX0.STROBE_START_DELAY