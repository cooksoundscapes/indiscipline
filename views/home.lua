local knob = require("knob")
local k1 = add_component({
    x = 50,
    y = 50,
    w = 50,
    h = 50,
    level = 30,
    draw = function(self)
        --[[Color("#ff0000")
        rectangle(0, 0, 50, 50)
        fill()]]
        knob(25, 25, 45, 0, 100, self.level, "#fafafa", "#2e2e2e", "#ee2233")
    end
})
