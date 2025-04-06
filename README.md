# Hello Instance Rendering

This is a small project created for the purpose of practicing what I've learned in [LearnOpenGL](https://learnopengl.com/Advanced-OpenGL/Instancing). It showcases the rendering of 1 million instanced cubes.

![GIF of project running](https://github.com/PsionicAlch/hello-instance-rendering/blob/main/assets/screenshots/screenshot.gif)

## Build Instructions for Hello Instance Rendering

### Step 1. Install Dependencies

Before building, ensure you have the required dependencies:

- CMake (version 3.30 or newer)
- C++ Compiler (GCC, Clang, MSVC, or MinGW)
- Git (to clone the repository)

#### Window:

- Install [CMake](https://cmake.org/)
- Install [Visual Studio](https://visualstudio.microsoft.com/) with the C++ Development Tools **OR** Install [MinGW-w64](https://www.mingw-w64.org/) for MinGW builds

#### MacOS (Using Homebrew):

```bash
brew install cmake ninja
```

#### Linux (Ubuntu/Debian-based):

```bash
sudo apt update
sudo apt install cmake g++ make clang git
```

### Step 2. Clone the Repository (With Submodules)

Since the project uses submodules (e.g., GLFW, GLM), make sure to clone it with the --recursive flag:

```bash
git clone --recursive https://github.com/PsionicAlch/hello-instance-rendering.git
```

If you already cloned the repository without --recursive, you can manually initialize submodules with:

```bash
cd hello-instance-rendering
git submodule update --init --recursive
```

### Step 3. Create a Build Directory

Navigate to the project directory and create a separate build folder:

```bash
cd hello-instance-rendering
mkdir -p build
cd build
```

### Step 4. Configure the Project with CMake

Run cmake to generate the build files.

#### Windows (Visual Studio)

```bash
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
```

You can also use "MinGW Makefiles" if using MinGW.

#### MacOS/Linux

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Step 5. Build the Project

Now, compile the project using the generated build system.

#### Windows (Visual Studio)

```bash
cmake --build . --config Release
```

#### MacOS/Linux

```bash
cmake --build . -- -j$(nproc)  # Use all CPU cores for faster builds
```

### Step 6. Run the Executable

After a successful build, the compiled executable can be found in:

```bash
build/output/HelloInstanceRendering
```

#### Run the Application

**Windows**

```bash
output\HelloInstanceRendering.exe
```

**MacOS/Linux**

```bash
./output/HelloInstanceRendering
```

### Step 7. Keeping the Repository Updated

To update your local copy, pull the latest changes and update submodules:

```bash
git pull --recurse-submodules
git submodule update --recursive --remote
```

### Troubleshooting

#### CMake Not Found?

Ensure CMake is installed and available in your system’s PATH. Try running:

```bash
cmake --version
```

If it doesn’t work, reinstall CMake or add it to your PATH.

#### Build Errors?

- Ensure you have the correct dependencies installed.
- Try deleting the build folder and reconfiguring:

```bash
rm -rf build  # macOS/Linux
rmdir /s /q build  # Windows (PowerShell)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```