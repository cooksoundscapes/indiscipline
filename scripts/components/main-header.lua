local header = {
  ["cpu"] = function()
    local command = "ps -eo pcpu | awk 'NR>1' | awk '{sum += $1} END {print sum}'"
    local handle = io.popen(command)
    if handle == nil then
      return 0
    end
    local result = handle:read("*a")
    handle:close()

    -- The result will be a string containing the total CPU usage percentage
    return tonumber(result)
end
}