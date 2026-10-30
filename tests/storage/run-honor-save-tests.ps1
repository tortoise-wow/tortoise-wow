param([string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/quest-mail/honor'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) { throw 'Load MSVC x64 environment first.' }
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/W4','/UNDEBUG',
    (Join-Path $PSScriptRoot 'honor_save_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'honor-save.obj')),
    ('/Fe' + (Join-Path $testOutput 'honor-save-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'honor-save-test.exe')
if ($LASTEXITCODE) { throw "Honor save checks failed: $LASTEXITCODE" }
