local knob = require("knob")
local fps_counter = require("lib.fps")

local knob_array = {}

for i=0,3 do
    knob_array[i+1] = add_component({
        x = (i%4)*50 + 10,
        y = (math.floor(i/4))*70 + 10,
        w = 50,
        h = 70,
        level = 50,
        draw = function(self)
            knob(self.w/2, self.h/2, 45, 0, 100, self.level, "#fafafa", "#000000", "#ee2233")
            move_to(15, 55)
            Color("#ffffff")
            text(string.format("%d", self.level), 10)
        end
    })
end

--[[
local oscilo = add_live_component({
    x = 10,
    y = 130,
    w = 200,
    h = 100,
    draw = function(self)
        local buffer = get_audio_buffer(1)

        Color("#fafafa")
        rectangle(0, 0, self.w, self.h)
        stroke()
        move_to(0, 0)
        text(string.format("%.2f",fps_counter:get()), 10)

        move_to(0, self.h/2)
        local d = #buffer / self.w
        for i,s in pairs(buffer) do
            line_to(i*d, -(s*self.h/2)+self.h/2)
        end
        stroke()
    end
})
]]

function PanelInput(device, pin, state)
    if device == "encoders" and knob_array[pin] ~= nil then
        local trgt = knob_array[pin]
        trgt:set("level", trgt.level + state)
    end
end
