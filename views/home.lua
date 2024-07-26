local function rms(buffer)
    local rms = 0
    for i,s in ipairs(buffer) do
        rms = rms + (s ^ 2)
    end
    rms = math.sqrt(rms/#buffer)
    return rms
end

function Draw()
   local b = get_audio_buffer(1)
   Color("#ffffff")
   text(string.format("%.2fdB", rms(b)))
end