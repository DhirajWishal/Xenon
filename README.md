# Xenon

Cross-platform graphics engine.

## Why Xenon?

This engine is almost entirely a learning experience for me. It's meant to be a testing ground to make graphics engines and all the other support systems. I don't think anyone would want to use this engine
other than to see what it's like to have support for multiple backends with multithreading and to have support for multiple backends. I'm planning on adding support for Android as well but I'll get to it when
we can actually render a proper scene with post-processing. Right now it only support Windows and has a Vulkan and DirectX 12 backend (which can be selected at runtime).

## How to build?

Start off by cloning the repository to a local directory. You can follow the following commands to get started.

```sh
git clone https://github.com/DhirajWishal/Xenon
cd Xenon
git submodule update --init --recursive
./Scripts/Bootstrap.[bat/ sh]
```

*Note: Use the proper shell script for the platform. For Windows it's the `Bootstrap.bat` file and for Linux it's the `Bootstrap.sh` file.*
*Also the submodule update will also download glTF samples which is like 2 gigabytes.*

The scripts will create the output directory and the CMake project files. From there on it's just a CMake build to build the binaries and run!

## How to use it with a project?

If you are crazy enough to use this in a project, look into the Studio project's `/Studio/CMakeLists.txt` file to get started. You can easily add this repo as a submodule if you want which will configure
a lot of things for you. Right now we don't have an option to install Xenon (because it's not my biggest concern yet). Just make sure that you set the include path to `/Engine/` and link the `XenonEngine`
static library file (which will be in the `/Build/Engine/Xenon/` folder).

## License

This project is licensed under Apache-2.0.
