
local lunajson = require("lunajson")

function test()
    print("Hai!")
    local kek = getSampleJson()
    print(lunajson.encode(kek))
end

