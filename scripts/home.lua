-- home page will receive input only through PanelInput!

LastPage = "home"
local running_patch = ""
local pd_patches = {}

function pd_init()
	local ls_out = io.popen("ls ~/pd/*.pd")
	if ls_out ~= nil then
		for line in ls_out:lines() do
			local filename = line:match(".+/([^/]+)")   -- Get the last part after the last '/'
			local no_extension = filename:match("(.+)%..+")

			table.insert(pd_patches, {
				name=no_extension,
				action = function(name)
					if name == running_patch then
						load_module(app, LastPage)
					else
						os.execute("killall pd")
						os.execute("pd -nogui ~/pd/"..name..".pd &")
						running_patch = name
					end
				end
			})
		end
	end
end

pd_init()

local menu = require("components.menu")
menu.entries = {
	{
		name = "load patch",
		submenu=pd_patches,
	},
	{
		name = "system",
		submenu = {
			{
			  name = "start DHCP",
			  action = function()
				os.execute("sudo dhcpcd")
			  end
			},
			{
			  name = "stop DHCP",
			  action = function()
				os.execute("sudo dhcpcd -k")
			  end
			},
			{
			  name = "restart system",
			  action = function()
				os.execute("reboot")
			  end
			},
			{
			  name = "shutdown",
			  action = function()
				os.execute("poweroff")
			  end
			},
		},
	},
}

function Draw()
  menu:draw()
end

local buttons = {
	function()
		load_module(app, LastPage)
	end,
	function()
		menu:back()
	end,
	function()
		menu:call()
	end
}

function PanelInput(device, pin, value)
  if device == "encoders" and pin == 1 then
    menu:select(value)
    return
  end
  if value == 1 and device == "buttons" and pin >= 1 and pin <= 3 then
    buttons[pin]()
  end
end
