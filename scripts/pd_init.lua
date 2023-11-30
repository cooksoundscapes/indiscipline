local function pd_init(running_patch)
  local pd_patches = {}
	local file_handler = io.popen("ls ~/pd/*.pd")
	if file_handler ~= nil then
		for line in file_handler:lines() do
			local file_name = line:match(".+/([^/]+)")   -- Get the last part after the last '/'
			local patch_name = file_name:match("(.+)%..+") -- strip file extension

			table.insert(pd_patches, {
				name = patch_name,
				action = function(name)
					if name == running_patch then
						load_module(app, LastPage)
					else
						os.execute("killall pd")
						os.execute("pd -nogui ~/pd/"..name..".pd &")
						-- TODO: auto connect pd output to this app
						running_patch = name
					end
				end
			})
		end
	end
  return pd_patches
end

return pd_init