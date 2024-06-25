## Indiscipline

C++ Engine + Cairo graphics + Lua scripting + OSC server/client + SPI/I2C/GPIO handling (for raspberry pi);
* Can be compiled to draw on SDL2 window, linux framebuffer or SPI (depends on custom display adaptation) - Already works for SSD1306


### Adding new lua-registered cairo calls
1. add declaration at `cairo-wrapper.h`
2. define at `cairo-wrapper.cpp`
3. define wrapper function at `lua-functions.cpp`
4. declare the extern wrapper function at `lua-runner.h`
5. call `loadFunction` at `lua-runner.cpp` inside `LuaRunner::init` call


### OSC endpoints:
* `/navigate <string>`
* `/param <any>` - any number of string/float values, evokes `SetParam` lua call (user defined)
* `/buffer <string> <...float>` - table name followed by N float values, evokes `SetTable` lua call (user defined)
* `/panel <string> <float> <float>` - physical panel mockup, args are device/pin/value
* `/reset` reloads lua state
* `/print` Cairo will attempt to save next frame in home dir as PNG