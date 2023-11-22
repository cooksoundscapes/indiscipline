#pragma once

//--------------GENERAL----------------//
constexpr const char* SEND_OSC = "send_osc";
constexpr const char* HOME_PAGE = "home";
constexpr const char* DIRECT_CONTROL = "direct-ctrl";
constexpr const char* LUA_UTILS = "utils";
constexpr const char* OSC_CLIENT = "7778";
constexpr const char* OSC_SERV = "7777";
constexpr int A_CHANNELS = 10;

constexpr int WINDOW_WIDTH = 320;
constexpr int WINDOW_HEIGHT = 240;
constexpr int OLED_DISPLAY_WIDTH = 128;
constexpr int OLED_DISPLAY_HEIGHT = 64;

//--------------- LUA GLOBALS-----------------------//
constexpr const char* SCREEN_W = "screen_w";
constexpr const char* SCREEN_H = "screen_h";
constexpr const char* SET_PARAM = "SetParam";
constexpr const char* PANEL_INPUT = "PanelInput";
constexpr const char* DRAW = "Draw";
constexpr const char* CLEANUP = "Cleanup";
constexpr const char* SET_TABLE = "SetTable";

//----------------------HW-------------------//

constexpr const char* I2C_DEVICE = "/dev/i2c-1";
constexpr int RESETPIN = 22;
constexpr int DCPIN = 23;
constexpr int ENCODER_INT = 17;
constexpr int BUTTON_INT = 27;