# wukong
## Introduction

OpenHarmony stability testing automation tool simulates disorderly user behavior to stress test the stability of OpenHarmony systems and applications.<br>

wukong component architecture diagram<br>
![架构图](figures/wukongArchitectureDiagram.png)<br>

Submodule responsibilities within the component：<br>
1. Command line parsing: allows you to obtain and parse parameters on the command line.<br>
2. Running environment management: Initialize the overall wukong running environment according to the command line.<br>
3. System interface management: Check and get the specified mgr, register controller and dfx faultlog callback functions.<br>
4. Random event generation: Generate a random sequence with a specified number of seeds through the random function to generate events.<br>
5. Event injection: Inject events into the system based on supported event types, relying on subsystems such as Windows, multimode, and security.<br>
6. Exception capture and processing/report generation: The DFX subsystem obtains anomaly information in operation and records logs to generate reports.<br>

## Directory

```
├── wukong                              # wukong main code file
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
│   ── BUILD.gn                         # Store the configuration of wukong construction, including construction object, method, dependency, hardware architecture, and file format
│   ── README_zh.md                     # The readme file
```

## Constraints

1. wukong began presetting after system version 3.2<br>
2. Versions of wukong prior to version 3.2 do not compile with the version. When using wukong, you need to compile and push it to the OpenHarmony device under test. The procedure is as follows：<br>
    2.1. Build a way
    ```
    ./build.sh --product-name rk3568 --build-target wukong
    ```
    2.2. push
    ```
    hdc_std shell mount -o rw,remount /
    hdc_std file send wukong /
    hdc_std shell chmod a+x /wukong
    hdc_std shell mv /wukong /bin/
    ```

## Directions for use

### Function Features and Command Description


| Command | Description | Note |
| -------------- | ---------------------------------------------- | ------------- |
| wukong version | Get wukong version information | -v, --version |
| wukong help    | Get wukong help information |               |
| wukong appinfo | Query support pulling up the application bundleName and the corresponding mainAbility name |               |
| wukong special | wukong special test |               |
| wukong exec    | wukong randomly tests |               |


### wukong special description

| Command | Features | Required | Note |
| :------------------ | ---------------------- | ---- | :------------------ |
| -h, --help          | Get help information for the current test. | No | Specialized test help information. |
| -k, --spec_insomnia | Sleep wake-up special test. | No | -                   |
| -c, --count         | Set the Number of executions. | No | The default is 10 times. |
| -i, --interval      | Set the execution interval. | No | Unit ms, default 1500ms. |
| -S, --swap          | Sliding test. | No | -                   |
| -s, --start[x,y]    | Set the slide test start coordinates. | No | -                   |
| -e, --end[x,y]      | Set the slide test end coordinates. | No | -                   |
| -b, --bilateral     | Set the round-trip swipe. | No | By default, there is no round-trip swipe. |
| -t, --touch[x,y]    | Click test. | No | -                   |
| -T, --time          | Set the total test time. | No | Unit minutes, the default is 10 minutes. |
| -C, --component     | The control sequentially traverses the test. | No | You need to set the test app name. |

#### Example of wukong Special test
```bash
> hdc_std shell
# wukong special -C [bundlename] -p
```
Specific test example parsing：
| Command           | Parameter Value     | Description                                           |
| -------------- | -------------- | ---------------------------------------------- |
| wukong special |  | The main commands.                             |
| -C [bundlename] |[bundlename]| Control to sequentially traverse the test parameter Settings, bundlename is the name of the test application.    |
| -p | | Represents a screenshot.                             |

### wukong random description

| Command | Features | Required | Note |
| --------------- | ------------------------------------ | ---- | ---------------------------------------- |
| -h,--help       | Get help information for the current test. | No | Random test help information. |
| -c,--count      | Set the Number of executions, conflicts with -T. | No | The number of units, the default is 10 times. |
| -i,--interval   | Set the execution interval. | No | Unit ms, default 1500ms. |
| -s,--seed       | Set the Random Seed. | No | Configuring the same random seed results in the same random event sequence. |
| -b,--bundle[bundlename,......,bundlename]     | Set the list of allowed applications for this test, and the -p conflict. | No | By default, test all apps for the current device (app names separated by commas). |
| -p,--prohibit[bundlename,......,bundlename]   | Set the list of prohibited apps for this test, which conflicts with -b. | No | By default, no applications are prohibited (application names are separated by commas). |
| -a,--appswitch  | Set the app random pull-up test scale. | No | The default 10%. |
| -t,--touch      | Set the screen random touch test ratio. | No | The default 10%. |
| -S,--swap       | Set the screen random swap test scale. | No | The default 3%. |
| -m,--mouse      | Set the screen randommou test ratio. | No | The default 1%. |
| -k,--keyboard   | Set the screen random keyboard test ratio. | No | The default 2%. |
| -H,--hardkey    | Set the random hardkey test scale. | No | The default 4%. |
| -C, --component | Set the Random Control Test Scale. | No | The default 70%. |
| -T,--time       | Set the total test time, conflicts with -c. | No | Unit minutes, the default is 10 minutes. |

> Instructions: Configuring the same random seed results in the same sequence of random events

#### Example of wukong exec random test
```bash
> hdc_std shell
# wukong exec -s 10 -i 1000 -a 0.28 -t 0.72 -c 100
```
Random test example parsing：
| Command           | Parameter Value     | Description                                           |
| -------------- | --------------     | ---------------------------------------------- |
| wukong exec |       | The main command.                             |
| -s  | 10    | Parameter set random seed, 10 is the seed value.           |
| -i  | 1000   | Parameter Settings apply pull up interval, 1000 unit ms. |
| -a  | 0.28   | Parameter Settings Apply random pull up test ratio 28%.         |
| -t  | 0.72   | Parameter Settings Screen random touch test proportion is 72%.    |
| -c  | 100  | Parameter Setting The number of execution times is 100.                |

## Release notes

3.2.0.0 Release content: Preset wukong supports the following functions:<br>
1. Support the whole machine application pull up, set the random seed, set the application pull up interval, set the application pull up times, support the query application pull up bundle name and ability name.<br>
2. Support random injection of events, support random injection of controls, support sleep and wake up special tests, support control sequence traversal screenshots special tests.<br>
3. Supports wukong run log printing.<br>
4. White and blacklist applications are supported.