function Draw()
  set_source_rgb(1, 1, 1)
  move_to(0, 0)
  text(".::ANY_UI::.")

  move_to(0, H(0.5))
  set_line_width(1)
  local b = get_audio_buffer(app, 1)

  local jump = screen_w / get_buffer_size(app)

  for i,n in ipairs(b) do
    line_to(i*jump, n*H(0.5) + H(0.5))
  end
  stroke()

end
