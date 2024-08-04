local time = require("lib.time")
return {
    f = 0,
    lasttime = time.ms() / 1000,
    fps = 0,

    get = function(self)
        self.f = self.f + 1
        local currtime = time.ms() / 1000
        local elapsed = currtime - self.lasttime
        if elapsed >= 1 then
            self.fps = self.f / elapsed
            self.f = 0
            self.lasttime = currtime
        end
        return self.fps
    end
}