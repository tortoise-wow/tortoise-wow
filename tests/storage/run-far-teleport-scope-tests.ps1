param([string]$OutputDirectory = (Join-Path $env:TEMP 'mangos-storage-far-teleport'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) { throw 'Load MSVC x64 environment first.' }
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$sourceFile = Join-Path $PSScriptRoot '../../src/game/Maps/MapManager.cpp'
$sourceText = [IO.File]::ReadAllText((Resolve-Path $sourceFile).Path)
$start = $sourceText.IndexOf('void MapManager::ScheduleFarTeleport(')
$end = $sourceText.IndexOf('// Execute all delayed teleports', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Exact production scheduler body unavailable.' }
$productionBody = $sourceText.Substring($start, $end - $start)
[IO.File]::WriteAllText((Join-Path $testOutput 'tested-far-teleport.inc'), $productionBody)
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/W4','/UNDEBUG',
    ('/I' + $PSScriptRoot), ('/I' + $testOutput),
    (Join-Path $PSScriptRoot 'far_teleport_scope_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'far-teleport.obj')),
    ('/Fe' + (Join-Path $testOutput 'far-teleport-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'far-teleport-test.exe')
if ($LASTEXITCODE) { throw "Far-teleport scope checks failed: $LASTEXITCODE" }
