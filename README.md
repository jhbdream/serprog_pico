### 基于树莓派pico的spi norflasn烧写工具

- 依赖flashrom进行开发,在pico实现serprog的协议
- 通过usb连接pc上位机
- 通过spi接口链接spi flash芯片

#### flashrom 常用命令

```shell
flashrom -p serprog:dev=/dev/ttyACM0:4000000
```
