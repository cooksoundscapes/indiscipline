## Indiscipline

C++ Engine + Cairo graphics + Lua scripting + OSC server/client + SPI/I2C/GPIO handling (for raspberry pi);
* Draws both on SDL2 window or framebuffer


### Adding new lua-registered cairo calls
1. add declaration at `cairo-wrapper.h`
2. define at `cairo-wrapper.cpp`
3. define wrapper function at `lua-functions.cpp`
4. declare the extern wrapper function at `lua-runner.h`
5. call `loadFunction` at `lua-runner.cpp` inside `LuaRunner::init` call