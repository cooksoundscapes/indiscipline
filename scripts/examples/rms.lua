local buff = {}
local bname = "wvfrm"

local w = W(0.4)
local h = H(0.5)

local function render()
  create_surface(bname, w, h)
  set_source_rgb(1, 1, 1)
  rectangle(0, 0, w, h)
  stroke()

  local jump = w / #buff
  for i,n in ipairs(buff) do
    rectangle((i-1)*jump, h/2-(n*h/2), jump, n*h)
    fill()
  end
end

render()

function SetTable(buffer, name)
  buff = buffer
  render()
end

function SetParam(name, value)
  if name == "w" then
    w = value
  elseif name == "h" then
    h = value
  end
  render()
end

function Draw()
  local x, y = Center(w, h)
  set_source_rgb(1, 1, 1)
  draw_surface(bname, x, y)
end

function Cleanup()
  destroy_surface(bname)
end