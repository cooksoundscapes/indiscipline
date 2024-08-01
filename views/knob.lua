return function(x, y, size, min, max, level, border_color, bg_color, pointer_color)
  set_line_width(2)
  new_path()
  arc(x, y, size/2, 0, math.pi*180)
  Color(bg_color)
  fill_preserve()
  Color(border_color)
  stroke()

  local minAngle = -math.pi*1.4
  local maxAngle = math.pi*0.4
  local v = (level - min) / (max - min)
  v = minAngle + v * (maxAngle - minAngle)
  local px = x + (size/2) * math.cos(v)
  local py = y + (size/2) * math.sin(v)
  local fx = x + .25 * (px - x)
  local fy = y + .25 * (py - y)
  move_to(fx, fy)
  Color(pointer_color)
  line_to(px, py)
  stroke()
end