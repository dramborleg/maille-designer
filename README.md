Maille Inlay Designer
----

## Overview

Maille Inlay Designer is a program for simplifying the process of designing chain maille inlays. It makes it easy to create and edit designs without the need for designing by hand or using over complicated software not meant for this sort of task. The aim is to keep it simple, eg Paint but with maille rings instead of pixels. Once a design is finished, you can easily get a count of how many rings of each color there are by exporting a color report to help the process of buying wire/rings to create the design.

## Tips

The color report writes a text file which contains the rgb values of each of the colors, as well as how many rings of that color there are in the design. The rgb value of a color is composed of three numbers, the red component, green component, and blue component, each ranging from 0 to 255. Since this results in ~16 million possible colors, colors which are close to each other but not exactly the same will show up as different colors in the color report (this is a feature, not a bug).

What this means for users is that using the color wheel to select colors is only recommended for introducing new colors to the design. If a ring already exists with the color you want to use, you should use the color select tool to set the current color so that new rings have exactly the same color as the existing rings. This will create a more coherent color report.

## Compiling

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
