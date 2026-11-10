param([string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/quest-mail/actor-scope'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) { throw 'Load MSVC x64 environment first.' }
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/W4','/UNDEBUG',
    ('/I' + $PSScriptRoot),
    (Join-Path $PSScriptRoot 'atomic_scope_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'atomic-scope.obj')),
    ('/Fe' + (Join-Path $testOutput 'atomic-scope-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'atomic-scope-test.exe')
if ($LASTEXITCODE) { throw "Atomic actor scope checks failed: $LASTEXITCODE" }
