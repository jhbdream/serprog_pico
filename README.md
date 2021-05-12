## 基于树莓派pico的spi norflasn烧写工具

- 依赖flashrom进行开发,在pico实现serprog的协议
- 通过usb连接pc上位机
- 通过spi接口链接spi flash芯片

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

### flashrom 常用命令

```shell
flashrom -p serprog:dev=/dev/ttyACM0:4000000

flashrom -p serprog:dev=/dev/ttyACM0:4000000,spispeed=1000 -c W25Q128.W --layout rom.layout --image test -w test.bin

```
