Color = {
  red = {hex("#ff0000")},
  blue = {hex("#0000ff")},
  green = {hex("#00ff00")},
  white = {hex("#ffffff")},
  black = {hex("#000000")}
}

function SetColor(rgb_table)
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

function PrintTable(table)
  for k, v in pairs(table) do
    print(k, v)
  end
end

-- placeholders to avoid accessing nil functions
function Draw() end
function SetParam(name, value) end
function Cleanup() end
function PanelInput(device, pin, value) end
function SetTable(table, name) end
package.path = "/home/me/views/?.lua;" .. package.path
