PrintInfo('ELUNA_RUNTIME_SMOKE_LOADED')

local function onStartup()
    PrintInfo('ELUNA_RUNTIME_SMOKE_WORLD_STARTED')
end

RegisterServerEvent(14, onStartup)
