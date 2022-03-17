# WuKong部件
## 工具介绍

OpenHarmony稳定性测试自动化工具，通过模拟无序的用户行为，对OpenHarmony系统及应用进行稳定性压力测试。

## 支持功能

> 当前版本支持如下功能，扩展能力后续规划持续改进

- 支持RK3568、Hi3516
- 支持Shell启动
- 支持整机应用拉起
- 支持设置随机种子，按固定随机序列拉起
- 支持设置应用拉起间隔，默认1000ms
- 支持设置应用拉起次数，默认次数10次
- 支持查询应用拉起bundle名、ability名
- 支持注入随机事件
  - Touch事件注入
  - Motion事件注入
- 支持wukong运行日志打印
- 支持屏蔽unvisible应用启动
- 支持单应用测试
- 支持休眠唤醒专项测试

---

## 使用方式

```bash
> hdc_std shell
# wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```

```bash
> hdc_std shell wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```

wukong exec 为主命令，-s 参数设置随机种子，10为种子值；-i 参数设置应用拉起间隔，1500单位ms，-c 参数设置执行次数

---

## 命令说明

| 命令           | 功能                                           |
| :------------- | :--------------------------------------------- |
| wukong version | 查询WuKong版本号                               |
| wukong help    | 查询WuKong帮助信息                             |
| wukong appinfo | 查询支持拉起应用entryName和对应的mainAbility名 |
| wukong exec    | 执行WuKong测试命令，和选项配合使用             |

---

## 选项说明

| 短选项 | 长选项          | 功能                         | 备注     |
| :----- | --------------- | :--------------------------- | :------- |
| -v     | --version       | 查询WuKong版本号             |          |
| -k     | --spec_insomnia | 设置休眠唤醒专项测试         |          |
| -h     | --help          | 查询WuKong帮助信息           |          |
| -u     | --touch_pos     | 设置单次touch事件            |          |
| -s     | --seed          | 设置随机种子，相同随机种子为相同随机事件序列    | 随机值   |
| -i     | --interval      | 设置执行间隔                 | 单位ms   |
| -c     | --count         | 设置执行次数                 | 单位次数 |
| -b     | --bundle        | 设置单应用测试               | 被测应用 |
| -a     | --appswitch     | 设置所有应用随机拉起测试比例 | 百分比   |
| -t     | --touch         | 设置屏幕随机touch测试比例    | 百分比   |
| -x     | --xSrc          | 设置滑动起点 x 坐标          | 像素     |
| -y     | --ySrc          | 设置滑动起点 y 坐标          | 像素     |
| -m     | --xDst          | 设置滑动终点 m 坐标          | 像素     |
| -n     | --yDst          | 设置滑动终点 n 坐标          | 像素     |


---

## 推送wukong至设备

> 稳定性测试自动化工具wukong暂时不随版本编译，使用时需自行编译后推送至被测OpenHarmony设备

### 构建方式

```shell
./build.sh --product-name rk3568 --build-target wukong
```

### 推送方式

```shell
hdc_std shell mount -o rw,remount /
hdc_std file send wukong /
hdc_std shell chmod a+x /wukong
hdc_std shell mv /wukong /bin/
```

### Reference

[monkey](https://github.com/aosp-mirror/platform_development/tree/master/cmds/monkey)