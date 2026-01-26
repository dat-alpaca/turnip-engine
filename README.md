[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ec020ba40c7b4754ab2975940e76f178)](https://app.codacy.com/gh/DatDarkAlpaca/turnip-engine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

# Turnip Engine
> Turnip is my favorite toy game engine. It features an unecessarily complicated RHI (a fancy way of saying it can run both Vulkan and Modern OpenGL), a suboptimal Lua engine, and several other cool things.

# Getting Started
Turnip doesn't have prebuilt binaries yet, so if you want to use it, you'll need to build it yourself (sorry).
Keep in mind that the latest version of this project has been only tested on [Pop!_OS 22.04 LTS](https://system76.com/pop/) and [Fedora Linux 43 (KDE Plasma Edition)](https://www.fedoraproject.org/). Both `X11` and `Wayland` display server protocols have been tested using a [GeForce RTX™ 3050](https://www.nvidia.com/en-us/geforce/graphics-cards/30-series/rtx-3050/), and [driver version 580.119.02](https://www.nvidia.com/en-us/drivers/details/259145/). The latest version of the engine only supports `Vulkan 1.3` and above, and `OpenGL 4.6` and above. The OpenGL implementation is more likely than not, not fully implemented at any given moment, as I prioritize making sure the Vulkan RHI implementation is up to date first (for the most part). Plans for supporting `OpenGL 3.3` exist. 

You can check whether or not your graphics card supports the Vulkan features this engine uses [here](http://www.vulkan.gpuinfo.org/displayreport.php?id=39663#extensions).

Mainly, it requires:
* VK_KHR_swapchain
* VK_KHR_synchronization2
* VK_KHR_dynamic_rendering

## Prerequisites
Building this project requires the following:

* [Lua](https://www.lua.org/download.html)
* [CMake](https://cmake.org/download/)
* [Vulkan SDK](https://vulkan.lunarg.com/) (If you are using the Vulkan build)

If you are using a Linux desktop distribution, you must install the following dependencies to build the project, as stated in the [GLFW documentation](https://www.glfw.org/docs/latest/compile.html#compile_deps_wayland):

* Debian-based:
```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev 
``` 

* Fedora-based:
```bash
sudo dnf install wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
```

Additionally, you need to install the Mesa drivers (If you are using the OpenGL build):
```bash
sudo dnf mesa-libGL mesa-libGL-devel
```

#### VulkanSDK environment variables:

In Windows, you may need to set the environment variable `VULKAN_SDK` to your Vulkan SDK installation (e.g., `C:/Vulkan/<version>`).

In Linux distributions, I suggest reading [this](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html). A similar guide is available for Windows, but major modifications are probably not needed. 

## Installation

1. **Clone the repository**
    ```bash
    git clone --recurse-submodules https://github.com/dat-alpaca/turnip-engine
    ```

    In case you forget the `--recurse-submodules` option, just run `git submodule update --init --recursive` afterwards.
    <br>

2. **Generate the project**
    ```bash
    cmake -G Ninja -S . -B build/turnip -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    ```
    Some other options can be set, such as:
    | Option Name             | Information                              | Default |
    | -------------           | -------------                            | :---:   |
    | -DTUR_BUILD_SANDBOX     | Builds the turnip sandbox program        | ON      |
    | -DTUR_BUILD_ENGINE      | Builds the turnip engine library         | ON      |
    | -DTUR_USE_GLFW*         | Uses the GLFW windowing implementation   | ON      |
    | -DTUR_USE_OPENGL        | Uses the OpenGL backend                  | ON      |
    | -DTUR_USE_VULKAN        | Uses the Vulkan backend                  | OFF     |

    \*_This option is a lie. Turnip doesn't support other platforms yet._

    You can also edit the [CMakePresets.json](https://github.com/dat-alpaca/turnip-engine/blob/main/CMakePresets.json) file directly.
    <br>

4. **Compile the project**
    Use the generated project files to build the project. You may use any combination of build arguments, but the only tested environment can be found under [CMakePresets.json](https://github.com/dat-alpaca/turnip-engine/blob/main/CMakePresets.json).

    If you intend to use it, first adjust the `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` according to your system filepaths. Then, run the `cmake --build build/turnip --parallel 12` command.

    For Visual Studio (the purple one) users, use the `Build All` option after opening the Visual Studio solution project, or run `msbuild` directly if you have the appropriate environment variables for that.
   
    After that, you should have a working copy under `build/turnip-sandbox/` and `build/turnip-engine`.

## Configuration
The engine requires a location file specified as its main constructor argument. There is a default file here: [`res/engine_config.json`](https://github.com/dat-alpaca/turnip-engine/blob/main/res/engine_config.json).

> [!WARNING]
> Changing the graphics API string (`windowingCapabilities/api`) will **not** change the actual graphics API used.
As the TurnipRHI system uses static polymorphism, the engine has to be recompiled with the correct arguments in order to change its underlying RHI. This option exists because I'm too lazy to remove it. The graphics API version does matter, though.
Check the options listed [here](#installation) for more information. 

# Using
Currently, there are no available examples nor documentation. The best you can do (which is highly discouraged) is to follow the `turnip-sandbox` project code source, and check the `project` folder.

The engine does not provide a single entry point to the user, so you must declare an instance of the engine, and a conscrete implementation of a `tur::View`. However, the View is designed to be as bare-bones as possible. It does not provide a renderer system nor scenes, albeit those are readily available for use.

That being said, I would highly advise against using the engine for the moment. I will provide code examples and (probably) a proper documentation in the future. In case you decide to ignore all those warnigs, the sandbox application already supports scene and asset loading, with an immediate-mode quad renderer, tilemap renderer, cubemap (skybox) renderer, and a mesh renderer. Apart from that, it also supports scripting and audio loading, so you can technically make a proper game already by editing the `project` files.

# License

This project is licensed under the [MPL-2.0 license](https://www.mozilla.org/en-US/MPL/2.0/) - see the [LICENSE](LICENSE) file for details. It is important to note that until [a certain commit](https://github.com/dat-alpaca/turnip-engine/commit/3e2776747c4ac76e5793da7dedc50eb300a1aa40), the project was licensed under the [MIT License](https://opensource.org/license/mit).
