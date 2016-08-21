--------------------------------------------------
-- Level code
-- Created @DATETIME@
--------------------------------------------------

@ONSTART{
-- Run code on level start
function onStart()
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

@ONEVENT{
-- Run code when internal event of the SMBX Engine has been triggered
-- eventName - name of triggered event
function onEvent(eventName)
    --Your code here
end
@}

