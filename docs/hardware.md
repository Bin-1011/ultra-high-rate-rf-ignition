# 硬件环境与引脚分配

## 1. 硬件选型

| 类别 | 型号 | 选型说明 |
| --- | --- | --- |
| 单片机 | STC15W4K56S4 | 1T 8051 内核，在性能、功能和外设集成度上较传统 51 单片机有明显提升，稳定可靠 |
| 主显示 | LCD12864（ST7920 控制器） | 带中文字库，可显示中文、英文字符与图形；STN 类型，适合多种环境 |
| 辅助显示 | 0.91 寸 OLED（SSD1306） | 128x32 像素，尺寸小、对比度高，适合空间受限设备 |
| 传感器 | YL-62 红外避障传感器模块 | 含一对红外发射/接收管，探测距离可由电位器调节；干扰小、便于装配 |
| 核心板 | M51S-D6 系统板 | 板载 CH340 USB 转串口、8 路 LED（P0 口）、若干板载按键 |
| 驱动/烧录 | CH340 USB 转串口驱动 + STC-ISP V6.90 | 通过串口冷启动下载程序 |

系统主时钟：MAIN_Fosc = 22118400L，即 22.1184 MHz（定义于 firmware/Keil_C51/delay.h）。
Keil 工程的 Device 字段记录为 STC15W4K32S4 Series，实际使用的芯片型号为 STC15W4K56S4
（见烧录记录 tools/烧录说明.md）。

## 2. 引脚分配

| 功能 | 引脚 | 方向/有效电平 | 定义位置 |
| --- | --- | --- | --- |
| 向上按键 | P2.4 | 输入，低电平有效 | global.h: sbit up = P2^4 |
| 向下按键 | P2.5 | 输入，低电平有效 | global.h: sbit down = P2^5 |
| 确认按键 | P2.6 | 输入，低电平有效 | global.h: sbit confirm = P2^6 |
| 取消按键 | P2.7 | 输入，低电平有效 | global.h: sbit cancel = P2^7 |
| 点火节点模拟 LED | P0.0 ~ P0.7 | 输出，低电平点亮 | LED.h: #define LED P0 |
| LCD12864 数据总线 | P6.0 ~ P6.7 | 双向 | LCD.h: #define LCD_Data P6 |
| LCD12864 RS | P4.0 | 输出 | LCD.h: sbit LCD_RS = P4^0 |
| LCD12864 RW | P4.1 | 输出 | LCD.h: sbit LCD_RW = P4^1 |
| LCD12864 E | P4.2 | 输出 | LCD.h: sbit LCD_E = P4^2 |
| LCD12864 PSB（并口/串口选择） | P4.3 | 输出，1 = 8 位并行 | LCD.h: sbit PSB = P4^3 |
| LCD12864 RESET | P4.4 | 输出，低电平复位 | LCD.h: sbit LCD_RESET = P4^4 |
| OLED 数据 SDIN | P1.3 | 输出（软件 I2C） | oled.h: sbit OLED_SDIN = P1^3 |
| OLED 时钟 SCL | P1.4 | 输出（软件 I2C） | oled.h: sbit OLED_SCL = P1^4 |
| 红外传感器状态指示 | P2.0 | 输出，低电平点亮（开发板自带 LED） | sensor.c |
| 板载按键 | P2.1 | 输入，低电平有效（进入传感器等待循环） | sensor.c |
| 红外传感器触发输入 | P3.2 | 输入，低电平有效（传感器检测到目标） | sensor.c |

端口初始化：main() 中把 P0~P7 的 PnM1、PnM0 全部清零，即所有端口均为**准双向口**模式。

## 3. 开发环境搭建

1. **安装 USB 转串口驱动**：核心板使用 CH340，运行驱动包中的 HL-340.EXE，点击 INSTALL，
   出现“驱动安装成功”即完成。
2. **向 Keil 添加 STC15 型号与头文件**：打开 STC-ISP（本次使用 V6.90），在右侧选择
   “Keil 仿真设置”，选择目标芯片型号后点击对话框，指定 Keil 安装路径并确定，
   提示“STCMCU 型号添加成功”即完成。
3. **编译**：用 Keil uVision5 打开 firmware/Keil_C51/超高射频火炮.uvproj，Rebuild。
4. **烧录**：STC-ISP 中选择型号 STC15W4K56S4 与生成的 .hex 文件，冷启动上电下载。

## 4. 功能与端口行为对照

- 传感器功能**开启**（sensor_switch == 0）：P2.0 输出低电平点亮状态指示灯；
  按下板载按键 P2.1 后进入等待循环，P3.2 每次被拉低即执行一次点火，
  期间可用取消键退出，剩余药量同步递减并在 OLED 上刷新。
- 传感器功能**关闭**（sensor_switch == 1）：P2.0 输出高电平关闭状态指示灯，仅接受按键控制。
- 点火动作：led_single() 将 0x01 << led_count 取反后写入 P0，被选中一位输出低电平点亮 LED，
  延时 frequency 毫秒后把 P0 恢复为 0xFF 熄灭，led_count 在 0~7 之间循环。