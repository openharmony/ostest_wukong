# AGENTS.md — wukong（OpenHarmony 稳定性测试工具）

## 1. 项目定位

本文件适用于 `test/ostest/wukong`。

wukong 是 OpenHarmony 系统组件（子系统 `ostest`，部件 `wukong`），用于模拟无序用户行为对系统和应用进行稳定性压测。**不能脱离 OpenHarmony 完整源码树独立编译或运行**。

C++ 工程，命名空间 `OHOS::WuKong`，入口：`shell_command/src/wukong_main.cpp`。

## 2. 代码地图

| 目录 | 职责 | 操作提示 |
|------|------|----------|
| `shell_command/` | CLI 解析；`WuKongShellCommand` 继承 `OHOS::AAFwk::ShellCommand` | 新增子命令须在此注册命令映射 |
| `test_flow/` | 测试执行流：`TestFlow`（基类）→ `RandomTestFlow`/`SpecialTestFlow`/`FocusTestFlow`；由 `TestFlowFactory` 按 `exec`/`special`/`focus` 字符串分发 | 新增测试流须继承 `TestFlow` 并在 `TestFlowFactory::GetTestFlow` 中注册 |
| `input_factory/` | 事件注入工厂：`InputFactory` 按 `InputType` 枚举分发 `InputAction` 子类（touch、swap、keyboard、mouse、rotate、hardkey、component、knuckle、pinch、browser 等） | 新增注入事件须继承 `InputAction` 并在 `InputFactory::GetInputAction` 中注册 |
| `component_event/` | UI 组件树遍历与焦点场景委派；`WuKongTree`→`AbilityTree`→`PageTree`→`ComponentTree` 四层树结构 | 修改树结构需关注 `TreeManager` 和 `SceneDelegate` 联动 |
| `common/` | 跨模块基础设施：单例管理器（`AppManager`/`ComponentManager`/`MultimodeManager`/`WuKongUtil`）、日志（`WuKongLogger`）、工具函数 | 管理器均为单例，通过 `GetInstance()/DestroyInstance()` 控制 |
| `report/` | 异常捕获（`ExceptionManager` + `SysEventListener`）、统计（`Statistics` 系列子类）、报表输出（`Format`→`FormatCsv`/`FormatJson`/`FormatHtml`/`FormatTerminal`） | 新增统计维度须继承 `Statistics` 并在 `Report` 中集成 |

### 任务-路径快速索引

- 新增 CLI 子命令 → `shell_command/include/wukong_shell_command.h` + `shell_command/src/wukong_shell_command.cpp`
- 新增测试流 → `test_flow/include/` + `test_flow/src/` + `test_flow_factory.cpp` 注册
- 新增注入事件 → `input_factory/include/` + `input_factory/src/` + `input_factory.cpp` 注册 + `common/include/wukong_define.h` 的 `InputType` 枚举
- 修改组件树遍历逻辑 → `component_event/src/tree_manager.cpp` + `scene_delegate.cpp`
- 修改报表格式 → `report/src/format*.cpp` + `report/src/statistics*.cpp`
- 修改构建依赖 → 根目录 `BUILD.gn` + `bundle.json`

## 3. 约束与边界

### 架构不变量

- wukong 进程通过命名信号量（`SEMAPHORE_RUN_NAME`）保证单实例运行，同一设备仅允许一个 wukong 进程。
- 需开发者模式（`const.security.developermode.state` 为 true），非开发模式直接退出。
- 所有 Manager 类均为单例，生命周期由 `wukong_main.cpp` 的 `FreeSingtion()` 统一销毁。
- `TestFlowFactory` 仅识别 `exec`、`special`、`focus` 三个命令字符串，未知命令返回 `nullptr`。

### 外部源依赖

`BUILD.gn` 编入了一个外部源文件：
```
//foundation/ability/ability_runtime/tools/aa/src/shell_command.cpp
```
此文件来自 OpenHarmony 树内 `ability_runtime` 子系统，**不是本仓库代码**，不可在本仓库中修改。

### 不要做

- 未经明确要求，不要修改 `bundle.json` 中的部件依赖列表或 `BUILD.gn` 中的 `external_deps`。
- 不要在本仓库中编辑 `ability_runtime` 的 `shell_command.cpp`。
- 不要引入新的第三方生产依赖。
- 不要移除 `-fno-rtti` 或 `-flto` 编译选项（会影响产物大小与性能）。

## 4. 编译构建

所有命令需在 OpenHarmony 源码树根目录执行。

```bash
# 编译 wukong 可执行文件
./build.sh --product-name rk3568 --build-target wukong

# 编译单元测试
./build.sh --product-name rk3568 --build-target wukong_ut
```

- GN 目标路径：`//test/ostest/wukong:wukong`（可执行文件）、`//test/ostest/wukong/test:unittest`（单元测试）
- 单元测试源码：`test/unittest/wukong_test.cpp`，使用 googletest
- 编译产物路径：`out/rk3568/ostest/wukong/`

## 5. C++ 规范

- 编译选项：`-D__OHOS__ -Os -fno-rtti -fdata-sections -ffunction-sections -flto`；C++ 异常启用 `-fexceptions`
- 模块结构统一：`模块/include/*.h` + `模块/src/*.cpp`
- 所有源文件必须携带 Apache 2.0 版权头（`Copyright Huawei Device Co., Ltd.`）
- 命名空间：`OHOS::WuKong`
- 日志宏：`DEBUG_LOG`、`TRACK_LOG`、`ERROR_LOG`（由 `WuKongLogger` 控制，默认 INFO 级别；`--debug` 切 DEBUG，`--track` 切 TRACK）

## 6. 设备部署（3.2 之前版本）

3.2 之前版本 wukong 未预置，需手动推送：

```bash
hdc_std shell mount -o rw,remount /
hdc_std file send wukong /
hdc_std shell chmod a+x /wukong
hdc_std shell mv /wukong /bin/
```

## 7. 完成标准

1. 编译无错误无新增告警
2. 单元测试通过（`wukong_ut`）
3. 不包含无关格式化、重构或顺手改动
