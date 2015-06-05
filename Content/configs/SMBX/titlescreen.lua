

function __native_event(...)
    local args = {...}
    local i = 0
    for _, argvalue in pairs(args) do
        i = i + 1
        Logger.debug("Arg #"..i.." ("..type(argvalue)..": "..tostring(argvalue)) 
    end
end


