# audio_visualizer
audio_visualizer is a basic program written in C++ with vulkan and GLFW dependencies, that visualizes 
frequencies received from samples using fft.

## Usage
Program should be passed one argument that is path to audio file e.g.  
./audio_visualizer example.wav

## Building

### Prerequisites
To build following project it's necessary to install/build VulkanSDK (https://vulkan.lunarg.com) and glfw (https://www.glfw.org/download.html)

### Setting up CMakeLists
In CMakeLists.txt set paths to vulkan and GLFW library directories in CONFIG INFORMATION section  

### Shaders
In Order to rebuild shaders set glslc.exe filepath to .bat script

## Supported file formats
* FLAC
* WAV
* MP3

## Dependencies
* vulkan - https://www.vulkan.org
* GLFW - https://www.glfw.org
* miniaudio - https://miniaud.io