# Mes_Imgui_template

Build Demo Video: https://www.youtube.com/watch?v=s8-ZJp3GqcE

The name of the `include` folder in the video is changed to `3rdparty`.

This is a template of Imgui + Implot Project.  

On Windows10, it builed by Vscode, CMake. On Linux it just builed by CMake.

If you wanna use this template, you should copy the `lib` file and dll file by yourself.

# Dependencies

+ OpenGL    
    On Windows, you can download it in Visual Studio.
+ glfw  
    On Windows, you have to build it manually.
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

## glad

1. There is a `CMakeLists.txt` in the `3rdparty/windows/glad`, copy it to somewhere first, then delete the `3rdparty/windows` folder in the project.

2. Download the glad correspond to your Opengl Version on the [glad loader website](https://glad.dav1d.de/). Then move the `glad` folder into `3rdparty/windows`.

## MSVC

```
mkdir build && cd build
cmake ..
cmake --build
```

## mingw64

I'm not quite familar with mingw, but this is how I compile with mingw64:

```
C:\mingw64\bin\cmake.EXE --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=C:\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\mingw64\bin\g++.exe -SD:/document/GitHub/Mes_Imgui_template -Bd:/document/GitHub/Mes_Imgui_template/build -G "MinGW Makefiles"

cd build
cmake --build .
```

The path of my project was `D:\document\GitHub\Mes_Imgui_template`, and the path of my mingw was `C:\mingw64`, please changed the path to yours.