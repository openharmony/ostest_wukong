# WuKong部件
## 简介

OpenHarmony稳定性测试自动化工具，通过模拟用户行为，对OpenHarmony系统及应用进行稳定性压力测试。

## 代码目录

```
├── wukong                              # wukong主要代码文件
|   └── common                          # 提供应用管控能力，随机事件注入能力，多模事件注入能力
|   └── component_event                 # 定义ability、page、Component树，提供添加节点、遍历树、依据NodeId查找子节点等能力
|   └── input_factory                   # 实现屏幕点击、滑动、拖拽、键盘等事件注入能力    
|   └── report                          # 监听异常信息，对其进行收集、统计、显示功能
|   └── shell_command                   # 用来创建命令行map,解析命令行参数并执行命令行               
|   └── test_flow                                  
│       └── include                     # 定义头文件
│       └── src                     
│           ├── random_test_flow.cpp    # 继承TestFlow,是随机测试的执行流
│           ├── special_test_flow.cpp   # 继承TestFlow,是顺序专项测试的执行流
│           ├── test_flow.cpp           # 检查命令行参数是否符合规范
│   ── BUILD.gn                         # 存放wukong构建的配置，具体包括构建对象、方式、依赖、硬件架构、文件格式
│   ── README_zh.md                     # readme文件
```

## 约束条件

1. WuKong在3.2系统版本后开始预置使用。<br>
2. WuKong在3.2系统版本之前的版本不随版本编译，使用时需自行编译后推送至被测OpenHarmony设备，步骤如下：        
    2.1. 构建方式
    ```
    ./build.sh --product-name rk3568 --build-target wukong
    ```
    2.2. 推送方式
    ```
    hdc_std shell mount -o rw,remount /
    hdc_std file send wukong /
    hdc_std shell chmod a+x /wukong
    hdc_std shell mv /wukong /bin/
    ```

## 功能特性

### 功能特性及命令说明


| 命令           | 说明                                           | 备注          |
| -------------- | ---------------------------------------------- | ------------- |
| version | 获取wukong版本信息。                             | -v, --version。 |
| help    | 获取wukong帮助信息。                             |              |
| appinfo | 查询支持拉起应用entryName和对应的mainAbility名。 |               |
| special | wukong专项测试。                                   |               |
| exec    | wukong随机测试。                                   |               |


### wukong special描述

| 命令                | 功能                   | 必选 | 备注                |
| :------------------ | ---------------------- | ---- | :------------------ |
| -h, --help          | 获取当前测试的帮助信息。 | 否   | 专项测试帮助信息。    |
| -k, --spec_insomnia | 休眠唤醒专项测试。       | 否   | -                   |
| -c, --count         | 设置执行次数。           | 否   | 默认10次。            |
| -i, --interval      | 设置执行间隔。           | 否   | 单位ms，默认1500ms。  |
| -S, --swap          | 滑动测试。               | 否   | -                   |
|-s, --start[x,y]    | 设置滑动测试起点坐标。   | 否   | -                   |
| -e, --end[x,y]      | 设置滑动测试终点坐标。   | 否   | -                   |
| -b, --bilateral     | 设置往返滑动。           | 否   | 默认不往返滑动。      |
| -t, --touch[x,y]    | 点击测试。               | 否   | -                   |
| -T, --time          | 设置测试总时间。         | 否   | 单位分钟，默认10分钟。 |
| -C, --component     | 控件顺序遍历测试。       | 否   | 需要设置测试应用名称。 |
| -r, --record     | 录制。       | 否   | 需要指定录制文件。 |
| -R, --replay    |  回放。      | 否   | 需要指定回放文件。 |
| -p, --screenshot    |  控件测试截图。      | 否   | - |

#### wukong special专项测试使用示例
```bash
> hdc_std shell
# wukong special -C [bundlename] -p
```
专项测试示例解析：
| 命令           | 参数值           | 说明                                           |
| -------------- |-------------- | ---------------------------------------------- |
| wukong special |  | 主命令。                             |
| -C [bundlename]    |[bundlename] | 控件顺序遍历测试参数设置，bundlename为测试应用名称。            |
| -p |  | 表示截图。                             |

### wukong random描述

| 命令            | 功能                                 | 必选 | 备注                                     |
| --------------- | ------------------------------------ | ---- | ---------------------------------------- |
| -h,--help       | 获取当前测试的帮助信息。               | 否   | 随机测试帮助信息。                         |
| -c,--count      | 设置执行次数。                         | 否   | 单位次数，默认10次。                       |
| -i,--interval   | 设置执行间隔。                         | 否   | 单位ms，默认1500ms。                       |
| -s,--seed       | 设置随机种子。                         | 否   | 配置相同随机种子，会生成相同随机事件序列。 |
| -b,--bundle[bundlename,……,bundlename]     | 设置本次测试的允许应用名单，与-p冲突。 | 否   | 默认测试当前设备所有应用(应用名称用逗号隔开)。                 |
| -p,--prohibit[bundlename,……,bundlename]   | 设置本次测试的禁止应用名单，与-b冲突。 | 否   | 默认不禁止任何应用(应用名称用逗号隔开)。                       |
| -a,--appswitch  | 设置应用随机拉起测试比例。             | 否   | 默认10%。                                  |
| -t,--touch      | 设置屏幕随机touch测试比例。            | 否   | 默认10%。                                  |
| -S,--swap       | 设置屏幕随机swap测试比例。             | 否   | 默认3%。                                   |
| -m,--mouse      | 设置屏幕随机mouse测试比例。            | 否   | 默认1%。                                   |
| -k,--keyboard   | 设置屏幕随机keyboard测试比例。         | 否   | 默认2%。                                   |
| -H,--hardkey    | 设置随机hardkey测试比例。              | 否   | 默认4%。                                   |
| -C, --component | 设置随机控件测试比例。                 | 否   | 默认70%。                                  |
| -T,--time       | 设置测试总时间。                       | 否   | 单位分钟，默认10分钟。                      |

> 说明：配置相同随机种子，会生成相同随机事件序列

#### wukong special随机测试使用示例
```bash
> hdc_std shell
# wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```
随机测试示例解析：
| 命令           | 参数值           | 说明                                           |
| -------------- | -------------- | ---------------------------------------------- |
| wukong exec |           | 主命令。                             |
| -s     | 10           | 参数设置随机种子，10为种子值。            |
| -i  | 1000           | 参数设置应用拉起间隔，1000单位ms。 |
| -a  | 0.28          | 参数设置应用随机拉起测试比例28%。          |
| -t  | 0.72           | 参数设置屏幕随机touch测试比例为72%。    |
| -c  | 100           | 参数设置执行次数为100次。                |

## 版本说明

3.2.0.0 版本发布内容：预置wukong，支持以下功能：<br>
1. 支持整机应用拉起、设置随机种子、设置应用拉起间隔、设置应用拉起次数、支持查询应用拉起bundle名和ability名。<br>
2. 支持随机注入事件、支持随机注入控件、支持休眠唤醒专项测试、支持控件顺序遍历截图专项测试。<br>
3. 支持wukong运行日志打印。<br>
4. 支持应用白、黑名单。