local menu = {
  entries = {},
  selected = 1,
  max_items_in_view = 4,
  offset = 0,
  submenu = nil,
  parents = {},

  call = function(self)
    local entry = {}
    if self.submenu == nil then
      entry = self.entries[self.selected]
    else
      entry = self.submenu[self.selected]
    end

    if entry.submenu ~= nil then
      self.set_submenu(self, entry.submenu, self.submenu or self.entries)
    else
      entry.action(entry.name)
    end
  end,

  select = function(self, dir)
    local size = 0
    if self.submenu == nil then
      size = #self.entries
    else
      size = #self.submenu
    end

    self.selected = math.max(1, math.min
      (size, self.selected + dir))

    self.offset = math.max(0,
      math.min(size - self.max_items_in_view, self.selected - self.max_items_in_view)
    )
  end,

  set_submenu = function(self, submenu, parent_menu)
    self.selected = 1
    self.offset = 0
    table.insert(self.parents, parent_menu)
    self.submenu = submenu
  end,

  back = function(self)
    if #self.parents == 0 then return end
    self.selected = 1
    self.offset = 0
    if #self.parents == 1 then
      table.remove(self.parents, #self.parents)
      self.submenu = nil
      return
    end
    self.submenu = self.parents[#self.parents]
    table.remove(self.parents, #self.parents)
  end,

  draw = function(self)
    SetColor(Color.white)
    local current_menu = {}
    if self.submenu ~= nil then
      current_menu = self.submenu
    else
      current_menu = self.entries
    end
    for i=self.offset+1,
      math.min(#current_menu, self.max_items_in_view+self.offset)
    do
      move_to(0, (i - 1 - self.offset) * 12)
      if self.selected == i then
        text("> " .. current_menu[i].name)
      else
        text(current_menu[i].name)
      end
    end
  end
}

return menu
