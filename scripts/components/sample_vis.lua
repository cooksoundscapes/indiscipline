local sample_vis = {
  name = "",
  w = 0,
  h = 0,
  buffer = {},
  render = function(self)
    create_surface(self.name, self.w, self.h)
    set_source_rgb(1, 1, 1)
    rectangle(0, 0, self.w, self.h)
    stroke()
  
    local jump = self.w / #self.buffer
    for i,n in ipairs(self.buffer) do
      rectangle((i-1)*jump, self.h/2-(n*self.h/2), jump, n*self.h)
      fill()
    end
  end,

  new = function(self, obj)
    obj = obj or {}
    setmetatable(obj, self)
    self.__index = self
    return obj
  end
}

return sample_vis