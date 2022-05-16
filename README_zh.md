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
  - Swap事件注入
  - Mouse事件注入
  - Keyboard事件注入
  - Hardkey事件注入
- 支持wukong运行日志打印
- 支持休眠唤醒专项测试
- 支持应用白名单
- 支持应用黑名单
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



| 命令           | 说明                                           | 备注          |
| -------------- | ---------------------------------------------- | ------------- |
| wukong version | 获取wukong版本信息                             | -v, --version |
| wukong help    | 获取wukong帮助信息                             | -h, --help    |
| wukong appinfo | 查询支持拉起应用entryName和对应的mainAbility名 |               |
| wukong special | 悟空专项测试                                   |               |
| wukong exec    | 悟空随机测试                                   |               |

----
## 命令选项描述

### wukong special描述

| 命令                | 功能                   | 必选 | 备注                |
| :------------------ | ---------------------- | ---- | :------------------ |
| -h, --help          | 获取当前测试的帮助信息 | 否   | 专项测试帮助信息    |
| -k, --spec_insomnia | 休眠唤醒专项测试       | 否   | -                   |
| -c, --count         | 设置执行次数           | 否   | 默认10次            |
| -i, --interval      | 设置执行间隔           | 否   | 单位ms，默认1500ms  |
| -S, --swap          | 滑动测试               | 否   | -                   |
| -s, --start[x,y]    | 设置滑动测试起点坐标   | 否   | -                   |
| -e, --end[x,y]      | 设置滑动测试终点坐标   | 否   | -                   |
| -b, --bilateral     | 设置往返滑动           | 否   | 默认不往返滑动      |
| -t, --touch[x,y]    | 点击测试               | 否   | -                   |
| -T, --time          | 设置测试总时间         | 否   | 单位分钟，默认10分钟 |
| -C, --component     | 控件顺序遍历测试       | 否   | 暂不支持            |

### wukong random描述

| 命令            | 功能                                 | 必选 | 备注                                     |
| --------------- | ------------------------------------ | ---- | ---------------------------------------- |
| -h,--help       | 获取当前测试的帮助信息               | 否   | 随机测试帮助信息                         |
| -c,--count      | 设置执行次数                         | 否   | 单位次数，默认10次                       |
| -i,--interval   | 设置执行间隔                         | 否   | 单位ms，默认1500ms                       |
| -s,--seed       | 设置随机种子                         | 否   | 配置相同随机种子，会生成相同随机事件序列 |
| -b,--bundle[bundlename,……,bundlename]     | 设置本次测试的允许应用名单，与-p冲突 | 否   | 默认测试当前设备所有应用(应用名称用逗号隔开)                 |
| -p,--prohibit[bundlename,……,bundlename]   | 设置本次测试的禁止应用名单，与-b冲突 | 否   | 默认不禁止任何应用(应用名称用逗号隔开)                       |
| -a,--appswitch  | 设置应用随机拉起测试比例             | 否   | 默认10%                                  |
| -t,--touch      | 设置屏幕随机touch测试比例            | 否   | 默认10%                                  |
| -S,--swap       | 设置屏幕随机swap测试比例             | 否   | 默认3%                                   |
| -m,--mouse      | 设置屏幕随机mouse测试比例            | 否   | 默认1%                                   |
| -k,--keyboard   | 设置屏幕随机keyboard测试比例         | 否   | 默认2%                                   |
| -H,--hardkey    | 设置随机hardkey测试比例              | 否   | 默认4%                                   |
| -C, --component | 设置随机控件测试比例                 | 否   | 默认70%（暂不支持）                      |
| -T,--time       | 设置测试总时间                       | 否   | 单位分钟，默认10分钟                      |

> 备注：配置相同随机种子，会生成相同随机事件序列

---

## 推送wukong至设备

> 稳定性测试自动化工具wukong暂时不随版本编译，使用时需自行编译后推送至被测OpenHarmony设备

### 构建方式

```
./build.sh --product-name rk3568 --build-target wukong
```

### 推送方式

```
hdc_std shell mount -o rw,remount /
hdc_std file send wukong /
hdc_std shell chmod a+x /wukong
hdc_std shell mv /wukong /bin/
```

### Reference

[monkey](https://github.com/aosp-mirror/platform_development/tree/master/cmds/monkey)