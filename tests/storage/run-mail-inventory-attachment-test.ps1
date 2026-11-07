param([string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/quest-mail/actor-scope'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) { throw 'Load MSVC x64 environment first.' }
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/W4','/UNDEBUG',
    ('/I' + (Join-Path $PSScriptRoot 'mail-stubs')),
    (Join-Path $PSScriptRoot 'mail_inventory_attachment_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'mail-inventory-attachment.obj')),
    ('/Fe' + (Join-Path $testOutput 'mail-inventory-attachment-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'mail-inventory-attachment-test.exe')
if ($LASTEXITCODE) { throw "Inventory-attachment ownership regression failed: $LASTEXITCODE" }
