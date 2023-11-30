-- home page will receive input only through PanelInput!

LastPage = "home"


local pd_init = require("pd_init")
local current_patch = ""
local pd_patches = pd_init(current_patch)

local current_view = "home"
local general_file_handler

local menu = require("components.menu")
menu.entries = {
	{
		name = "test",
		action = function()
			general_file_handler = io.popen("~/longtest.sh")
			current_view = "test"
		end
	},
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
	settings = {
		{
			name = "OSC settings",
			action = function()
				current_view = "osc_settings"
			end
		}
	}
}

local views = {
	home = function()
		menu:draw()
	end,
	osc_settings = function()
		text("OSC settings")
	end,
	dhcp = function()
		text("DHCP")
	end,
	shutting_down = function()
		text("Shutting down...")
	end,
	rebooting = function()
		text("Rebooting...")
	end,
	restart_jack = function()
		text("Restarting Jack...")
	end,
	test = function()
		if general_file_handler ~= nil then
			local output = general_file_handler:read("*a")
			text(output)
			if output == "3" then
				current_view = "home"
				general_file_handler:close()
			end
		end
	end,
}

function Draw()
	views[current_view]()
end

local buttons = {
	function() -- home button
		load_module(app, LastPage)
	end,
	function() -- "left" button
		if current_view == "home" then
			current_view = "osc_settings"
		else
			current_view = "home"
		end
		menu:back()
	end,
	function() -- "right" button
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
