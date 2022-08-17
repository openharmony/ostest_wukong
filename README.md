# WuKong
## One、Introduction

OpenHarmony stability testing automation tool simulates disorderly user behavior to stress test the stability of OpenHarmony systems and applications.

## Two、code directory

```
├── wukong                              # Wukong main code file
|   └── common                          # Provides application control capabilities, random event injection capabilities, multi-mode event injection capabilities
|   └── component_event                 # Define the ability, page, Component tree to provide the ability to add nodes, traverse the tree, find child nodes by NodeId, etc
|   └── input_factory                   # Realize the screen click, slide, drag, keyboard and other events injection ability    
|   └── report                          # Monitor abnormal information, collect, collect statistics, and display it
|   └── shell_command                   # Used to create a command line map, parse the command line parameters, and execute the command line              
|   └── test_flow                                  
│       └── include                     # Defining Header files
│       └── src                     
│           ├── random_test_flow.cpp    # Inherited from TestFlow, is the execution flow of random tests
│           ├── special_test_flow.cpp   # Inherited from TestFlow, is the execution flow of sequential specific tests
│           ├── test_flow.cpp           # Check whether the command line parameters conform to specifications
│   ── BUILD.gn                         # Store the configuration of WUkong construction, including construction object, method, dependency, hardware architecture, and file format
│   ── README_zh.md                     # The readme file
```

## Third、constraints

1、WuKong was prefabricated for use after system version 3.2
2、Versions of WuKong prior to version 3.2 do not compile with the version. When using WuKong, you need to compile and push it to the stabbed OpenHarmony device. The steps are as follows：
```
2.1、Build a way
    ./build.sh --product-name rk3568 --build-target wukong
2.2、push
    hdc_std shell mount -o rw,remount /
    hdc_std file send wukong /
    hdc_std shell chmod a+x /wukong
    hdc_std shell mv /wukong /bin/
```

## Four、functional characteristics

### Function Features and Command Description


| Command | Description | Note |
| -------------- | ---------------------------------------------- | ------------- |
| wukong version | Get wukong version information | -v, --version |
| wukong help    | Get wukong help information |               |
| wukong appinfo | Query support pulling up the application enterName and the corresponding mainAbility name |               |
| wukong special | wukong special test |               |
| wukong exec    | wukong randomly tests |               |

----

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

#### Example of Wukong Special test
```bash
> hdc_std shell
# wukong special -C [bundlename] -p
```
special test：wukong special main command. -C control iterates the test parameters in sequence. bundlename is the name of the test application. -p indicates the screenshot

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

#### Example of Wukong Special random test
```bash
> hdc_std shell
# wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```
Random test: wukong exec main command, -s parameter set random seed, 10 is the seed value; -i Specifies the application pull up interval, 1000 unit ms. -a Indicates the application random pull up test ratio of 28%. -t sets the proportion of random touch tests to 72%. -c Specifies the execution times to 100
---

## Five、Release Version releaseNote

1、3.2.0.0 version
> Content of the release：Pre-made Wukong supports the following functions
> 1、Support collecting app pull up, setting random seed, setting app pull up interval, setting app pull up times, and querying app pull up bundle name and ability name；
> 2、Support random injection of events, support random injection of controls, support sleep and wake up special tests, support control sequence traversal screenshots special tests；
> 3、Supports WUkong run log printing
> 4、White and blacklist applications are supported