param([string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/quest-mail'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) { throw 'Load MSVC x64 environment first.' }
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/W4','/UNDEBUG',
    ('/I' + (Join-Path $PSScriptRoot 'mail-stubs')),
    (Join-Path $PSScriptRoot 'prepared_mail_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'prepared-mail.obj')),
    ('/Fe' + (Join-Path $testOutput 'prepared-mail-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'prepared-mail-test.exe')
if ($LASTEXITCODE) { throw "Prepared mail checks failed: $LASTEXITCODE" }
