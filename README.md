# Mes_Imgui_template

Build Demo Video: https://www.youtube.com/watch?v=s8-ZJp3GqcE

This is a template of Imgui + Implot Project. 

On Windwos10, it builed by Vscode, CMake. On Linux it just builed by CMake.

If you wanna use this template, you should copy the `lib` file and dll file by yourself.

# Linux

just clone it and build it.

```bash
git clone https://github.com/Mes0903/Mes_Imgui_template.git
cd Mes_Imgui_template
mkdir build && cd build
cmake ..
cmake --build .
```

# Windows

You can check the Demo Video to help you build it, it's a little complicated.
## glfw

There is a lib file `glfw3.lib` in the `include/windows/lib`, you should build it by yourself. (Since the lib file on the github is depends on my environment.)

1. download the source code from [glfw](https://www.glfw.org/download).
2. make a `build` folder and get into it (You can do it by Vscode) 
3. cmake ..
4. Open the GLFW.sln, build the project by Visual Studio.
5. There would be a lib file called `glfw3.lib` in Debug folder, delete my `glfw3.lib` and copy your `glfw3.lib` into `include/windows/lib`.
6. Also, there is a folder called `GLFW` in the glfw `include/windows`, delete my `GLFW` and copy your `GLFW` into `include/windows`.

## OpenGL

There is a lib file `OpenGL32.Lib` in the `include/windows/lib`, you should build it by yourself. (Since the lib file on the github is depends on my environment.)

it's under the folder `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64` (or `x86`, depends on the edition you build), copy it to here.

## glad

1. There is a `CMakeLists.txt` in the `include/windows/glad`, copy it to somewhere first, then delete the `include/windows` folder in the project.

2. Download the glad correspond to your Opengl Version on the [glad loader website](https://glad.dav1d.de/). Then move the `glad` folder into `include/windows`.
