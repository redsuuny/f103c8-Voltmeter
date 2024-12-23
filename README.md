﻿# Voltmeter Project
## 项目概述
本项目是一个电压测量仪表，使用 A/D 转换器读取电压值，并在 LCD 屏幕上显示当前电压值（U）。该电压值保留小数点后两位。

## 目录结构
settings/
.vscode/
Core/
Debug/
Drivers/
ECUAL/
util/
README.md
STM32F103C8T6_FLASH.ld
lcdceshioc/

## 功能说明
1. **电压测量**  
   - 使用 A/D 系列电压值，将当前电压值显示在 LCD 屏幕上。电压值保留小数点后两位，超过设定值 2V 时，LED 灯闪烁报警。

2. **测量范围**  
   - 设备可测量的电压范围为 0V 至 3V。当电压达到 1.5V 时，设备会自动反转方向（180°）。

3. **LCD 显示**  
   - LCD 屏幕实时显示电压值，并提供调节亮度和对比度的功能，使用户能够轻松读取测量结果。

## 使用说明
1. **连接电源**  
   确保电压源在 3V 以内，接入设备。

2. **启动设备**  
   设备启动后，LCD 屏幕将显示当前电压值。

3. **注意事项**  
   - 设备每次测量电压时，精度为 0.01V，响应时间为 1 秒。


该项目包含用于控制步进电机和LCD16x2显示屏的驱动程序，基于STM32微控制器。
### `ECUAL/LCD16X2/LCD16X2.c`

该文件包含用于控制LCD16x2显示屏的驱动程序函数，包括初始化、写字符、写字符串等功能。
示例函数：
void LCD16X2_Write_Char(uint8_t LCD16X2_Index, char Data);
void LCD16X2_Write_String(uint8_t LCD16X2_Index, char *str);
包含用于控制步进电机的驱动程序函数，包括初始化、设置速度、步进控制等功能。
void STEPPERS_Init(void);
void STEPPER_SetSpeed(uint8_t au8_STEPPER_Instance, uint16_t au16_RPM);
void STEPPER_Step_Blocking(uint8_t au8_STEPPER_Instance, uint32_t au32_Steps, uint8_t au8_DIR);
将项目导入到STM32CubeIDE中。
根据硬件连接修改配置文件 STEPPER_cfg.h 和 LCD16X2_cfg.h。
编译并下载程序到STM32微控制器。

## 开发环境
- 硬件：STM32F103C8T6
- 开发工具：Keil / STM32CubeIDE
- 依赖库：CMSIS / HAL库

## 贡献
欢迎任何形式的贡献！请提交问题或拉取请求。

## 许可证
本项目采用 MIT 许可证，具体请见 LICENSE 文件。
