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
