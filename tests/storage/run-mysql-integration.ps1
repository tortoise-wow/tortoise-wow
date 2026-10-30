param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build'),
    [string]$OptionsFile = (Join-Path $PSScriptRoot '../../../runtime/secrets/database-admin.ini'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/mysql-integration'),
    [string]$RuntimeDllDirectory = 'D:/WoW/runtime/server'
)
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue) -or -not $env:ACE_ROOT) {
    throw 'Load the MSVC x64 build environment and ACE_ROOT before running this test.'
}
$sourceRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$buildRoot = (Resolve-Path -LiteralPath $BuildDirectory).Path
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$testOutput = (Resolve-Path -LiteralPath $OutputDirectory).Path
$argsForCompiler = @('/nologo','/std:c++17','/EHsc','/MD','/W3','/DWIN32',
    '/D_CRT_SECURE_NO_WARNINGS','/DFMT_HEADER_ONLY')
foreach ($include in @('src/shared','src/framework','dep/include','dep/fmt/include',
    'dep/windows/include/mysql','dep/windows/include/openssl','dep/re2')) {
    $argsForCompiler += '/I' + (Join-Path $sourceRoot $include)
}
$argsForCompiler += @("/I$env:ACE_ROOT/include", "/I$buildRoot",
    (Join-Path $PSScriptRoot 'mysql_integration_test.cpp'),
    "/Fo$testOutput/", "/Fe$testOutput/mysql-integration.exe", '/link',
    (Join-Path $buildRoot 'src/shared/Release/shared.lib'),
    (Join-Path $buildRoot 'src/framework/Release/framework.lib'),
    (Join-Path $buildRoot 'dep/re2/Release/re2.lib'),
    (Join-Path $sourceRoot 'dep/windows/lib/x64_release/libmysql.lib'),
    (Join-Path $sourceRoot 'dep/windows/lib/x64_release/libcrypto.lib'),
    "$env:ACE_ROOT/lib/ACE.lib", 'ws2_32.lib','winmm.lib','dbghelp.lib','shlwapi.lib','user32.lib')
& cl.exe @argsForCompiler *> (Join-Path $testOutput 'build.log')
if ($LASTEXITCODE) { throw "Integration compile failed; see $testOutput/build.log" }
$previousPath = $env:PATH
Push-Location $testOutput
try {
    $env:PATH = "$env:ACE_ROOT/bin;$RuntimeDllDirectory;" + $env:PATH
    & (Join-Path $testOutput 'mysql-integration.exe') $OptionsFile *> (Join-Path $testOutput 'results.log')
    if ($LASTEXITCODE) { throw "Integration failed; see $testOutput/results.log" }
    Get-Content -LiteralPath (Join-Path $testOutput 'results.log') -Tail 2
} finally {
    $env:PATH = $previousPath
    Pop-Location
}
