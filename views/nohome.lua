local knob = require("knob")
local fps = require("lib.fps")

local l = 0

function Draw()
    local f = fps:get()
    move_to(40, 20)
    Color("#ffffff")
    text(string.format("%.2f", f), 10)

    move_to(80, 20)
    text(l, 10)
end

local v = add_component({
    x = 10,
    y = 20,
    w = 80,
    h = 20,
    level = 0,
    draw = function(self)
        Color("#ffffff")
        move_to(0, 0)
        text(string.format("%d", self.level), 10)
    end
})

add_live_component({
    x = 100,
    y = 20,
    w = 80,
    h = 20,
    draw = function()
        local f = fps:get()
        Color("#ffffff")
        move_to(0, 0)
        text(string.format("%.2f", f), 10)
    end
})

function PanelInput(device, pin, state)
    if device == "encoders" and pin == 1 then
        l = l  + state
        v:set("level", v.level + state)
    end
end
