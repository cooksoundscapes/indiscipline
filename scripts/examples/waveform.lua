if Noiz == nil then
  Noiz = {}
end

local _date = io.popen("date")
local date = ""
if _date ~= nil then
  date = _date:read("*a")
end

local function render()
  create_surface("waveform", 80, 30)
  local jump = 80 / #Noiz
  rectangle(0, 0, 80, 30)

  move_to(0, H(0.5))
  set_line_width(1)
  for i,n in ipairs(Noiz) do
    line_to(i*jump, n*4 + 15)
  end
  stroke()
end

render()

function SetTable(buffer, name)
  if name == "noiz" then
    Noiz = buffer
    render()
  end
end

function Draw()
  local x, y = Center(80, 30)
  draw_surface("waveform", x, y)

  move_to(0, 0)
  set_source_rgb(1, 1, 1)
  text(date, 8, 0, 0)
end

function Cleanup()
  -- noiz = nil
  destroy_surface("waveform")
end