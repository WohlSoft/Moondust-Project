class 'lua_testclass'

function lua_testclass:__init()
    self.testVal = "HI :D"
end

function lua_testclass:logHi()
    Logger.debug("hi")
end

return lua_testclass