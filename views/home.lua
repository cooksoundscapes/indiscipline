local knob = require("knob")

local knob_arr = {}

for i=0,7 do
    knob_arr[i+1] = add_component({
        x = (i%4)*50 + 10,
        y = (math.floor(i/4))*50 + 10,
        w = 50,
        h = 50,
        level = 50,
        draw = function(self)
            knob(self.w/2, self.h/2, 45, 0, 100, self.level, "#fafafa", "#2e2e2e", "#ee2233")
        end
    })
end

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

        move_to(0, self.h/2)
        local d = #buffer / self.w
        for i,s in pairs(buffer) do
            line_to(i*d, -(s*self.h/2)+self.h/2)
        end
        stroke()
    end
})

function SetParam(k, lvl)
    knob_arr[k]:set("level", lvl)
    -- knob_arr[k] = lvl
end
