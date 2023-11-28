local audio = {
	rms = function(buffer)
		local rms = 0
		for i,s in ipairs(buffer) do
			rms = rms + (s ^ 2)
		end
		rms = math.sqrt(rms/#buffer)
		return rms
	end
}

return audio
