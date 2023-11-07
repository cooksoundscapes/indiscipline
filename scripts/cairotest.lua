local color = {
  red = {hex("#ff0000")},
  blue = {hex("#0000ff")},
  green = {hex("#00ff00")},
  white = {hex("#ffffff")},
  black = {hex("#000000")}
}
local function set_color(rgb_table)
  set_source_rgb(table.unpack(rgb_table))
end

local function w(ratio)
  return screen_w * ratio
end

local function h(ratio)
  return screen_h * ratio
end

local parameters = {}
function SetParam(key, value)
  parameters[key] = value
end

FPS_THROTTLE = 20
FPS_FRAME = 0
FPS = 0

function Draw()
  --set_color(color.black)
  --paint()

  set_color(color.white)
  move_to(0, 0)
  text(".::ANY_UI::.", 10, 0, 0)

  if fps ~= nil then
    if FPS_FRAME == 0 then
      FPS = fps
    end
    move_to(0, 18)
    text(FPS .. " fps", 10, 0, 0)
  end
  FPS_FRAME = (FPS_FRAME + 1) % FPS_THROTTLE

  move_to(0, h(0.5))
  set_line_width(2)
  local b = get_audio_buffer(caller, 1)
  for i,n in ipairs(b) do
    line_to(i, n*h(0.5) + h(0.5))
  end
  stroke()

  local cc = ""
  for k,v in pairs(parameters) do
    cc = cc .. k .. " : " .. v .. '\n'
  end
  move_to(w(0.2), h(0.2))
  text(cc, 18, 0, 0)

end
