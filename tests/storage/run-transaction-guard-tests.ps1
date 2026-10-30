param([string]$OutputDirectory = (Join-Path $env:TEMP 'mangos-storage-transaction-guard'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $vsPath = & $vsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsPath) { throw 'MSVC C++ tools are required.' }
    Import-Module (Join-Path $vsPath 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
    Enter-VsDevShell -VsInstallPath $vsPath -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'
}
$sourceRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo', '/std:c++17', '/EHsc', '/W3', '/DWIN32', '/D_CRT_SECURE_NO_WARNINGS',
    ('/I' + (Join-Path $PSScriptRoot 'stubs')),
    ('/I' + (Join-Path $sourceRoot 'dep/windows/include/mysql')),
    (Join-Path $PSScriptRoot 'transaction_guard_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'guard.obj')),
    ('/Fe' + (Join-Path $testOutput 'guard-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'guard-test.exe')
if ($LASTEXITCODE) { throw "Transaction guard tests failed: $LASTEXITCODE" }
