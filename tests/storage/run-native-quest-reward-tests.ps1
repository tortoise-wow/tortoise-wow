param([string]$OutputDirectory = (Join-Path $env:TEMP 'mangos-storage-native-quest-rewards'))
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $vsPath = & $vsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsPath) { throw 'MSVC C++ tools are required.' }
    Import-Module (Join-Path $vsPath 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
    Enter-VsDevShell -VsInstallPath $vsPath -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'
}
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$testOutput = (Resolve-Path $OutputDirectory).Path
$argsForCompiler = @('/nologo', '/std:c++17', '/EHsc', '/W4', '/UNDEBUG',
    (Join-Path $PSScriptRoot 'native_quest_reward_test.cpp'),
    ('/Fo' + (Join-Path $testOutput 'native-quest-rewards.obj')),
    ('/Fe' + (Join-Path $testOutput 'native-quest-rewards-test.exe')))
& cl.exe @argsForCompiler
if ($LASTEXITCODE) { throw "Compilation failed: $LASTEXITCODE" }
& (Join-Path $testOutput 'native-quest-rewards-test.exe')
if ($LASTEXITCODE) { throw "Native quest reward tests failed: $LASTEXITCODE" }
