local parameters = {}
function SetParam(key, value)
  parameters[key] = value
  if key == "gohome" then
    load_module(app, "home")
  end
end

FPS_THROTTLE = 20
FPS_FRAME = 0
FPS = 0

function Cleanup()
  FPS_FRAME = nil
  FPS_THROTTLE = nil
  FPS = nil
end

function Draw()
  --set_color(color.black)
  --paint()

  Set_color(Color.white)
  move_to(0, 0)
  text(".::ANY_CU::.")

  if fps ~= nil then
    if FPS_FRAME == 0 then
      FPS = fps
    end
    move_to(0, 18)
    text(FPS .. " fps")
  end
  FPS_FRAME = (FPS_FRAME + 1) % FPS_THROTTLE

  move_to(0, H(0.5))
  set_line_width(1)
  local b = get_audio_buffer(app, 1)
  for i,n in ipairs(b) do
    line_to(i, n*H(0.5) + H(0.5))
  end
  stroke()

  local cc = ""
  for k,v in pairs(parameters) do
    cc = cc .. k .. " : " .. v .. '\n'
  end
  move_to(W(0.2), H(0.2))
  text(cc, 18)

end
