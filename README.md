Sine Nomine
===========

Sine Nomine is a hobbyist exercise in C programming. It started as an exercise
in curses programming and might slowly evolve into a roguelike game.

Currently it looks like this:

![screenshot of the program](https://raw.githubusercontent.com/blabber/sine_nomine/master/screenshot.png "awesome")

## Building

To build this project on systems using a BSD-style make (`bmake(1)`):

```sh
% bmake all     # build the project (this is the default target)
% bmake debug   # build a debug version of the project
% bmake test    # run tests
% bmake clean   # remove build artifacts
```

For systems using GNU make (`gmake(1)`), a rudimentary `GNUmakefile` is
provided:

```sh
% gmake         # build the `sn` binary
```

Using `bmake(1)` is the recommended way to build this project. The
`GNUmakefile` was contributed and is not really maintained. It may stop working
without warning.
