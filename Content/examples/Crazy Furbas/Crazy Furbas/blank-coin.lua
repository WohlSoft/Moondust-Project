class 'BlankCoin'

function BlankCoin:__init(npc_obj)
    self.npc_obj = npc_obj
    self.contacts = npc_obj:installContactDetector()
    self.playerIsTouches = 0
end

function BlankCoin:onLoop(tickTime)
    local playersCount = 0;
    -- Detect contacts of playable characters
    if(self.contacts:detected())then
        local players = self.contacts:getPlayers()
        for K,Plr in pairs(players) do
            playersCount = playersCount + 1
        end
    end
    -- If count of touching players less than previous value
    if(self.playerIsTouches > playersCount)then
        -- Transform into regular coin
        self.npc_obj:transformTo(10, 1);
    end
    self.playerIsTouches = playersCount;
end

return BlankCoin

