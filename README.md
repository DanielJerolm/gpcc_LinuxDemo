# gpcc_LinuxDemo
Demo application using GPCC on Linux.

# Purpose
gpcc_LinuxDemo is intended to provide a starting point for evaluation of GPCC (https://github.com/DanielJerolm/gpcc) on
a Linux host.

The project contains GPCC and googletest as git submodules plus a demo application. The demo application demonstrates
some features of the CLI and can be extended by the user as needed in order to try out other features of GPCC.

The project uses two target environments and builds two artifacts:
- __Productive environment:__  
  An executable containing the demo application is build.  
  Ouput location: build_productive/output/demoApp  
  GPCC is configured to provide a _productive OSAL without any instrumentation_.  
- __Unittest environment:__  
  An executable containing unittest testcases from GPCC and from the demo application is build:  
  Output location: build_unittest/output/unittests  
  GPCC is configured to provide an _instrumented OSAL with TFC (Time flow control)_.

Note that the primary project for __development__ of GPCC is https://github.com/DanielJerolm/gpcc_dev which allows to
build GPCC in all configurations for all supported platforms, plus the unit tests for the host.

# System requirements
- Linux host (x64 or ARM) for building and executing artifacts
- Native gcc (8.0.0 or later recommended)
- cmake (3.13 or later)
- doxygen (1.9.1 or later recommended)
- vscode or an editor of your choice

# Usage
## Checkout
HTTPS:

`git clone -b main --recurse-submodules https://github.com/DanielJerolm/gpcc_LinuxDemo.git`

SSH:

`git clone -b main --recurse-submodules git@github.com:DanielJerolm/gpcc_LinuxDemo.git`

Git submodules are located in gpcc_LinuxDemo/extern.  
You may want to switch GPCC to a specific branch:  
```
$ cd extern/gpcc
$ git branch --all
$ git checkout <your choice>
```

## Build from console
Prerequisites:
- Successful checkout from git.

Build and start the productive executable:
```
$ cd scripts
$ ./cmake_init_productive_release.sh
$ ./build_productive.sh all

$ ../build_productive/output/demoApp
```

Build and run the unittest executable:
```
$ cd scripts
$ ./cmake_init_unittest_release.sh
$ ./build_unittest.sh all

$ ./execute_unittests.sh
```

If you encounter issues related to limited system resources or restricted user rights, some of GPCC's unittest cases
can be disabled in gpcc_LinuxDemo/cmake/CMakeLists.unittest.cmake, line 28-31.

## Build from vscode
Prerequisites:
- Successful checkout from git.
- vscode installed
- Folder "gpcc_LinuxDemo" opened in vscode

Build and start the productive executable:
1. Main menue "Terminal" -> Run Task... -> cmake_init_productive_release
2. Main menue "Terminal" -> Run Task... -> build (productive)
3. Open a terminal. Navigate to gpcc_LinuxDemo/build_productive/output and run "demoApp"

Build and run the unit tests:
1. Main menue "Terminal" -> Run Task... -> cmake_init_unittest_release
2. Main menue "Terminal" -> Run Task... -> build (unittest)
3. Main menue "Terminal" -> Run Task... -> Run all unittests

## Browse code with vscode
The project uses two target environments: Productive and unittest  
Source code is build with a different build configuration for each environment, e.g. `#defines` (-D) could be different.

To ensure that intellisense understands the code properly and does not display any error follow these advice:
- CMake initialization step has been done for both target environments
- Whenever a cpp- or h-file is open in the editor and has the focus, VSCODE will display the current configuration in
  the very bottom right.  
  Click on the label to select among `Linux-productive` and `Linux-unittests`.  
  Switch between the two configurations as needed when you work on unit test code or productive code.
