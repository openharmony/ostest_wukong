# WuKong

## Introduction

OpenHarmony stability testing automation tool simulates disorderly user behavior to stress test the stability of OpenHarmony systems and applications.

## Supported features

> The current version supports the following functions, and the expansion capacity is planned for continuous developing in the future.

- Support RK3568, Hi3516
- Support shell startup
- Support whole machine application pull up
- Support set random seeds, pull up by fixed random sequence
- Support setting app pull-up interval, default 1000ms
- Support setting the number of app pulls, the default number of times is 10 times
- Support query application to pull up bundle name, ability name
- Support injecting random events
  - Touch event injection
  - Swap event injection
  - Mouse event injection
  - Keyboard event injection
  - Hardkey event injection
- Support wukong run log printing
- Support sleep wake-up special test
- Support app whitelisting
- Support app blacklist

---

## How to use?

```bash
> hdc_std shell
# wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```

```bash
> hdc_std shell wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```

`wukong exec` is the main command, the `-s` parameter sets the random seed, and 10 is the seed value; `-i` sets application pull interval, 1500 units ms, -c parameter sets number of executions.

---

## Command Description

| Command | Description | Note |
| -------------- | ---------------------------------------------- | ------------- |
| wukong version | Get wukong version information | -v, --version |
| wukong help    | Get wukong help information | -h, --help    |
| wukong appinfo | Query support pulling up the application enterName and the corresponding mainAbility name |               |
| wukong special | wukong special test |               |
| wukong exec    | wukong randomly tests |               |

----

## Command option description

### wukong special description

| Command | Features | Required | Note |
| :------------------ | ---------------------- | ---- | :------------------ |
| -h, --help          | Get help information for the current test | No | Specialized test help information |
| -k, --spec_insomnia | Sleep wake-up special test | No | -                   |
| -c, --count         | Set the Number of executions | No | The default is 10 times |
| -i, --interval      | Set the execution interval | No | Unit ms, default 1500ms |
| -S, --swap          | Sliding test | No | -                   |
| -s, --start[x,y]    | Set the slide test start coordinates | No | -                   |
| -e, --end[x,y]      | Set the slide test end coordinates | No | -                   |
| -b, --bilateral     | Set the round-trip swipe | No | By default, slide the | round trip
| -t, --touch[x,y]    | Click test | No | -                   |
| -T, --time          | Set the total test time | No | Unit minutes, the default is 10 minutes |
| -C, --component     | The control sequentially traverses the test | No | You need to set the test app name |

### wukong random description

| Command | Features | Required | Note |
| --------------- | ------------------------------------ | ---- | ---------------------------------------- |
| -h,--help       | Get help information for the current test | No | Random test help information |
| -c,--count      | Set the Number of executions | No | The number of units, the default is 10 times |
| -i,--interval   | Set the execution interval | No | Unit ms, default 1500ms |
| -s,--seed       | Set the Random Seed | No | Configuring the same random seed results in the same random event sequence |
| -b,--bundle[bundlename,......,bundlename]     | Set the list of allowed applications for this test, and the -p conflict | No | By default, test all apps for the current device (app names separated by commas) |
| -p,--prohibit[bundlename,......,bundlename]   | Set the list of prohibited apps for this test, which conflicts with -b | No | By default, no apps (app names separated by commas) | are prohibited
| -a,--appswitch  | Set the app random pull-up test scale | No | The default 10% |
| -t,--touch      | Set the screen random touch test ratio | No | The default 10% |
| -S,--swap       | Set the screen random swap test scale | No | The default | is 3%.
| -m,--mouse      | Set the screen randommou test ratio | No | The default 1% |
| -k,--keyboard   | Set the screen random keyboard test ratio | No | The default | 2%.
| -H,--hardkey    | Set the random hardkey test scale | No | The default 4% |
| -C, --component | Set the Random Control Test Scale | No | The default 70% |
| -T,--time       | Set the total test time | No | Unit minutes, the default is 10 minutes |

> Note: Configuring the same random seed results in the same sequence of random events

---

## Push wukong to the device

> Stability test automation tool wukong is not compiled with the version for the time being, and it needs to be compiled by itself and pushed to the OpenHarmony device under test.

### Build

```
./build.sh --product-name rk3568 --build-target wukong
```

### Push

```
hdc_std shell mount -o rw,remount /
hdc_std file send wukong /
hdc_std shell chmod a+x /wukong
hdc_std shell mv /wukong /bin/
```