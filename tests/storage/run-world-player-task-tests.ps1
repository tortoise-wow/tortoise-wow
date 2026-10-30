param([string]$OutputDirectory = (Join-Path $env:TEMP 'mangos-world-player-tasks'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    throw 'Load the MSVC x64 build environment before running this test.'
}
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$testOutput = (Resolve-Path -LiteralPath $OutputDirectory).Path
$compilerArguments = @('/nologo','/std:c++17','/EHsc','/MD','/W4',
    (Join-Path $PSScriptRoot 'world_player_tasks_test.cpp'),
    (Join-Path $PSScriptRoot 'world_player_tasks_producer.cpp'),
    "/Fo$testOutput/", "/Fe$testOutput/world-player-tasks-test.exe")
& cl.exe @compilerArguments *> (Join-Path $testOutput 'build.log')
if ($LASTEXITCODE) { throw "Queue test compile failed; see $testOutput/build.log" }
& (Join-Path $testOutput 'world-player-tasks-test.exe') *> (Join-Path $testOutput 'results.log')
if ($LASTEXITCODE) { throw "Queue test failed; see $testOutput/results.log" }
Get-Content -LiteralPath (Join-Path $testOutput 'results.log')
