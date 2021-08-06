## introduction

- Look for PNGs in dir(default: `/usr/share/background`)
- Show PNGs in full screen
- Use font in `/usr/share/fonts` to show times
- Click the mouse will end the program

## requirements

- More than four PNGs
- PNG environment variable(default: `find /usr/share/backgrounds -name '*.png'`)
- Have fonts in `/usr/share/fonts`

```bash
$ sudo apt-get install libsdl2-dev
$ sudo apt-get install libsdl2-ttf-dev
$ sudo apt-get install libsdl2-image-dev
```

## build

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build . 
```

## run

```bash
$ cd build
$ export PNG="path/to/your/png/dir"
$ ./PNGShow
```
