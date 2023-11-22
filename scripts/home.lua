local menu = {
  entries = {
    {
      name = "reload patch",
      action = function()
        local r = io.popen("killall pd && pd -nogui ~/Pd/test.pd &&")
      end
    },
    {name = "MIDI options", action = function() print("midiopts") end}
  },
  selected = 1,
  call = function(self)
    self.entries[self.selected].action()
  end,
  select = function(self, dir)
    self.selected = math.max(1, math.min
    (#self.entries, self.selected + dir))
  end
}

LastPage = "cairotest"
local font = "Sans"
local fontSize = 10

function Draw()
  set_source_rgb(1, 1, 1)
  for i,entry in ipairs(menu.entries) do
    move_to(0, (i - 1) * 16)
    if menu.selected == i then
      text("> " .. entry.name, fontSize, font)
    else
      text(entry.name, fontSize, font)
    end
  end
end

function PanelInput(device, pin, value)
  if device == "encoders" and pin == 1 then
    menu:select(value)
    return
  end

  if value == 1 then
    if device == "buttons" then
      if pin == 1 then
        load_module(app, LastPage)
      elseif pin == 3 then
        menu:call()
      end
    end
  end

end

function Cleanup()
end