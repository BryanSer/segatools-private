# Segatools

## Compiling on Windows:

Prerequisites:

+ [Mingw-w64](http://mingw-w64.org/doku.php/download)
+ [Meson Build](https://mesonbuild.com/SimpleStart.html)
+ [Chocolatey](https://chocolatey.org/install)

Quick start on a Windows build host:

```
# To install Ninja on Windows:
$ choco install ninja

# Build with Meson and Ninja commands:
$ meson --cross cross-mingw-32.txt _build32
$ ninja -C _build32
$ meson --cross cross-mingw-64.txt _build64
$ ninja -C _build64

# To generate Visual Studio projects:
$ meson --backend vs2019 --cross cross-mingw-64.txt _build64
```

## Compiling on Linux:

Quick start on a Linux build host:

```
# Install Meson and a recent build of MinGW-w64, then:

$ meson --cross cross-mingw-32.txt _build32
$ ninja -C _build32
$ meson --cross cross-mingw-64.txt _build64
$ ninja -C _build64
```

Building on MSYS2 is also possible; consult Meson documentation for details.
