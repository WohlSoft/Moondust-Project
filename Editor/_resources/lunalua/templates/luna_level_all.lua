--------------------------------------------------
-- World map code
-- Created @DATETIME@
--------------------------------------------------
@ONLOAD{

-- Run code on world map loading by game engine before initalizing stuff
-- (CAREFULLY USE IT! Use it to initialize own variables
-- and don't process any operations with any objects and states of the level or world map!
-- Use onStart() for those purposes instead this).
function onLoad()
    --Your code here
end
@}
@ONSTART{

-- Run code on first frame of the world map
function onStart()
    --Your code here
end
@}
@ONLOOP{

-- Immediately at the start of each frame while the game runs onLoop is called.
-- Note: In LunaLua ≥ v0.7.3, you should usually use onTick in place of where you'd use onLoop in the past
function onLoop()
    --Your code here
end
@}
@ONTICK{

-- Run code every frame (~1/65 second)
-- (code will be executed before game logic will be processed)
function onTick()
    --Your code here
end
@}
@ONTICKEND{

-- Run code every frame (~1/65 second)
-- (code will be executed after game logic will be processed)
function onTickEnd()
    --Your code here
end
@}
@ONDRAW{

-- This is called just before drawing each frame
function onDraw()
    --Your code here
end
@}

