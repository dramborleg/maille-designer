Maille Inlay Designer
----

## Overview

Maille Inlay Designer is a program for simplifying the process of designing chain maille inlays. It makes it easy to create and edit designs without the need for designing by hand or using over complicated software not meant for this sort of task. The aim is to keep it simple, eg Paint but with maille rings instead of pixels. Once a design is finished, you can easily get a count of how many rings of each color there are by exporting a color report to help the process of buying wire/rings to create the design.

## Demo

[Here](https://www.youtube.com/watch?v=u9OGSniZnPs) is a demo of the program in action. It also contains a few suggestions for how to best utilize this software. [This](https://www.youtube.com/watch?v=cQ_QUGG56kM) is an older demo for v0.0.1.

## Issues

The main outstanding issue is the terrible performance when the inlay has many rings. I know where the bottleneck is, but fixing it will take some time so it may be a while before I get around to it. Until then, expect long delays for certain operations, expect it to use lots of RAM and CPU, and if you're on a laptop, you may get a hot computer if you use it for too long with very large designs. Hopefully it will still be useful for reasonably sized designs in the meantime.

## Compiling

Compiling should work on almost any system. I develop this on FreeBSD and Fedora and release official windows builds as well. I expect it would also compile for Macs but I do not own one and so have not tried it.

You'll need cmake version 3.5 or greater, a C++14 compiler, and all the necessary libraries for nanogui.

To compile on Fedora, install these packages

```bash
sudo dnf install cmake gcc-c++ mesa-libGLU-devel libXi-devel libXcursor-devel libXinerama-devel libXrandr-devel xorg-x11-server-devel
```

Once these are installed, the code can be built

```bash
git clone --recursive https://github.com/dramborleg/maille-designer.git
mkdir maille-designer/build && cd maille-designer/build
cmake ..
make
./maille
```

For finding the libraries to install on other systems, some combination of [the nanogui readme](https://github.com/wjakob/nanogui) and google will be good places to start.
