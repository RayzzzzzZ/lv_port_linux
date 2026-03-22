## Cursor Cloud specific instructions

This is **lv_port_linux** — the official LVGL Linux port. It builds a single native C binary (`build/bin/lvglsim`) that renders LVGL widget demos using various display backends (SDL2, fbdev, DRM, Wayland, X11, OpenGL ES).

### Build

See `README.md` for full instructions. Quick reference:

```
cmake -B build -S .
make -C build -j
```

The binary is at `./build/bin/lvglsim`.

### Running in Cloud VM

The default `lv_conf.h` enables `LV_USE_LINUX_FBDEV` and `LV_USE_EVDEV`. Since the Cloud VM has no physical framebuffer, **SDL2 must also be enabled** (`LV_USE_SDL 1` in `lv_conf.h`) for visual testing. Run with:

```
./build/bin/lvglsim -b sdl
```

Use `-B` to list compiled backends, `-b <name>` to select one.

### Key gotchas

- Git submodules (`lvgl`) must be initialized before building: `git submodule update --init --recursive`.
- CMake's LVGL integration requires `python3-venv` to preprocess `lv_conf.h`. Without it, `cmake` fails with a venv creation error.
- `libstdc++-14-dev` may need to be explicitly installed if the Clang linker fails to find `-lstdc++`.
- System deps for SDL backend: `libsdl2-dev`, `libsdl2-image-dev`. For evdev: `libevdev-dev`.
- The project has no automated test suite; verification is done by building and running the binary.
- Window size can be controlled via `LV_SIM_WINDOW_WIDTH` and `LV_SIM_WINDOW_HEIGHT` env vars (defaults: 800x480).
