Color = {
  red = {hex("#ff0000")},
  blue = {hex("#0000ff")},
  green = {hex("#00ff00")},
  white = {hex("#ffffff")},
  black = {hex("#000000")}
}

function Set_color(rgb_table)
  set_source_rgb(table.unpack(rgb_table))
end

function W(ratio)
  return screen_w * ratio
end

function H(ratio)
  return screen_h * ratio
end

function Center(width, height)
  return (screen_w - width)/2, (screen_h - height)/2
end

-- placeholders to avoid accessing nil functions
function Draw() end
function SetParam() end
function Cleanup() end
function PanelInput(device, pin, value) end
function SetTable(table, name) end