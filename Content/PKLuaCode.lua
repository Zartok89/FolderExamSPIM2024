function SpawnEntitiesFunction()
    -- Paramters (InLocationX, InLocationY, InVelocityX, InVelocityY, InAccelerationX, InAccelerationY)
    SpawnEntity(500, 4000, 0, 20, 20, 50)
    print("Spawned entity from Lua")
end

function GetPlayerLocationFunction()
    location = GetPlayerPawnLocation()
    print(location.x, location.y, location.z)
end

function SetEntityMaxSpeedFunction()
    local entitymaxspeed = SetEntityMaxSpeed(10000)
    print("Current max speed is: " .. entitymaxspeed)
end

function PrintNumberFunction()
    local printnumber = PrintNumber(50)
    print("from lua: " .. printnumber)
end
