# 3D-Raycaster-Engine

This is a 3D Raycaster Engine built in C using OpenGL.
It features a split-screen view that showcases both the underlying 2D game mechanics and the 3D rendered environment, illustrating how 2D data is used to create a 3D world.

![Screenshot](https://github.com/user-attachments/assets/a62e1e2f-1d50-456c-b3d7-6f81388ea0dc)

## Key Features
- **3D Rendering**: Renders a 3D environment using raycasting techniques.
- **OpenGL Integration**: Utilizes OpenGL for graphics rendering.
- **Customizable**: Easily modify and extend the engine for various use cases.

## Installation

Follow these steps to set up the environment on your Windows machine:

### 1. Install MinGW
- Download MinGW from [this link](https://sourceforge.net/projects/mingw/).
- If you need help with the setup, checkout this [tutorial guide](https://ics.uci.edu/~pattis/common/handouts/mingweclipse/mingw.html).

### 2. Set Up GL and GLUT

- The required OpenGL files are typically included with MinGW and should be located in the `C:\MinGW\include\GL` directory. If they are missing, refer to the [OpenGL website](https://www.opengl.org/) for installation instructions.

#### Steps for setting up GLUT:

1. **Download GLUT files**:
   - Download the necessary GLUT files from [this link](http://chortle.ccsu.edu/Bloodshed/glutming.zip).
   - Extract the contents of `glutming.zip`.

2. **Copy `glut.h` to the MinGW include folder**:
   - Locate `glut.h` in `GLUTMingw32\include\GL\`.
   - Copy `glut.h` to `C:\MinGW\include\GL\`.

3. **Copy `libglut32.a` to the MinGW lib folder**:
   - Locate `libglut32.a` in `GLUTMingw32\lib\`.
   - Copy `libglut32.a` to `C:\MinGW\lib\`.

4. **Copy `glut32.dll` to the SysWOW64 folder**:
   - Locate `glut32.dll` in `GLUTMingw32\`.
   - Copy it to `C:\Windows\SysWOW64\` (for 64-bit systems) or `C:\Windows\System32\` (for 32-bit systems).

### 3. Compile and Run the Raycaster
After completing the setup, clone this repository or download the `raycaster.c` file, then compile and run the program:

- Open a terminal in the project directory and compile the program with:
   ```bash
   gcc -o raycaster raycaster.c -IC:\MinGW\include -LC:\MinGW\lib -lopengl32 -lglu32 -lglut32
   ```

- Run the compiled executable with:
   ```bash
   raycaster.exe
   ```
