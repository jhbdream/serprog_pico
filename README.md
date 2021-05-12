## 使用树莓派pico的spi norflasn烧写工具

- 依赖flashrom进行开发,在pico实现serprog的协议
- 通过usb连接pc上位机
- 通过spi接口连接spi flash芯片,根据上位机指令实现flash烧写

---

### Serial Flasher Protocol Specification
flashrom 支持通过该协议与下位机设备通讯，实现flash的操作功能

具体的协议内容参考 `serprog-protocol.txt` 文件内容

该协议使用 命令-应答 的方式
- PC发出命令，命令可以携带数据作为参数，
- 设备做出应答 可以携带返回数据作为参数

根据命令与应答是否携带参数，可以将所有的命令分为几个类型

> 模式1
- 命令
- 应答

> 模式2
- 命令 + 数据
- 应答

> 模式3
- 命令
- 应答 + 数据

> 模式4
- 命令 + 数据
- 应答 + 数据

可以尝试抽象出一种通用的处理方法，对所有命令使用统一的处理函数

> 目前针对用到的命令抽象出来三种处理方法

- 普通命令 发送命令无参数， 返回ack有参数。多数简单命令可以再此通用处理
- spi操作命令，发送命令和返回都有参数 主要是数据读写
- 设置spi频率命令，发送命令和返回命令都有参数

---

### flashrom 常用命令

```shell
探测flash芯片
flashrom -p serprog:dev=/dev/ttyACM0:4000000

写入flash芯片 part1 地址范围内容
flashrom -p serprog:dev=/dev/ttyACM0:4000000,spispeed=31250000 --layout rom.layout --image part1 -w test.bin

擦除flash芯片 part1 地址范围内容
flashrom -p serprog:dev=/dev/ttyACM0:4000000,spispeed=31250000 --layout rom.layout --image part1 -E

读取flash芯片 part1 地址范围内容
flashrom -p serprog:dev=/dev/ttyACM0:4000000,spispeed=31250000 --layout rom.layout --image part1 -r file.bin
```


#### ROM layout文件

flashrom可以通过添加rom layout文件对flash地址划分进行描述，实现对局部内容操作

layout文件的格式如下:
```
00000000:00008fff part1
00009000:0003ffff part2
00040000:0007ffff part3

i.e.:
startaddr:endaddr name
```

- 使用 **--layout rom.layout** 指定使用的layout文件为rom.layout
- 使用 **--image name**  指定要操作的layout文件中的区域名称
- 使用 **-w filename** 将指定问价写入到flash中，可以通过layout文件实现局部写入，但是指定的文件大小需要与flash芯片容量大小相同 
- 使用 **-E**参数擦除flash芯片
- 使用**-r filename**命令读取flash芯片内容
- **spispeed=31250000** 指定spi时钟频率，需要下位机提供支持

---

#### 硬件说明：

```c
/* SPI pin define in CmakeLists.txt */
#define PIN_CLK 2
#define PIN_TX  3
#define PIN_RX  4
#define PIN_CS  5
```



#### 参考资料：

参考了基于stm32的flashrom项目实现：[stm32-vserprog](https://github.com/dword1511/stm32-vserprog)

