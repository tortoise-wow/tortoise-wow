param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../evidence/atomic-trade-database'),
    [string]$RuntimeDllDirectory = 'D:/WoW/runtime/server'
)
$ErrorActionPreference = 'Stop'
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue) -or -not $env:ACE_ROOT) {
    throw 'Load MSVC x64 and ACE_ROOT before running the focused scheduler test.'
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
$argsForCompiler += @("/I$env:ACE_ROOT/include", "/I$buildRoot")
foreach ($source in @('src/shared/Database/Database.cpp','src/shared/Database/SqlOperations.cpp',
    'src/shared/Database/SqlDelayThread.cpp','tests/storage/ordered_persistence_test.cpp')) {
    $argsForCompiler += Join-Path $sourceRoot $source
}
$argsForCompiler += @("/Fo$testOutput/", "/Fe$testOutput/ordered-persistence.exe", '/link',
    (Join-Path $buildRoot 'src/shared/Release/shared.lib'),
    (Join-Path $buildRoot 'src/framework/Release/framework.lib'),
    (Join-Path $buildRoot 'dep/re2/Release/re2.lib'),
    (Join-Path $sourceRoot 'dep/windows/lib/x64_release/libmysql.lib'),
    (Join-Path $sourceRoot 'dep/windows/lib/x64_release/libcrypto.lib'),
    "$env:ACE_ROOT/lib/ACE.lib", 'ws2_32.lib','winmm.lib','dbghelp.lib','shlwapi.lib','user32.lib')
& cl.exe @argsForCompiler *> (Join-Path $testOutput 'scheduler-build.log')
if ($LASTEXITCODE) { throw "Scheduler compile failed; see $testOutput/scheduler-build.log" }
$previousPath = $env:PATH
Push-Location $testOutput
try {
    $env:PATH = "$env:ACE_ROOT/bin;$RuntimeDllDirectory;" + $env:PATH
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo.FileName = Join-Path $testOutput 'ordered-persistence.exe'
    $process.StartInfo.UseShellExecute = $false
    $process.StartInfo.CreateNoWindow = $true
    $process.StartInfo.RedirectStandardOutput = $true
    $process.StartInfo.RedirectStandardError = $true
    $process.Start() | Out-Null
    $stdout = $process.StandardOutput.ReadToEndAsync()
    $stderr = $process.StandardError.ReadToEndAsync()
    if (-not $process.WaitForExit(15000)) {
        $process.Kill()
        throw 'Focused scheduler test exceeded 15 seconds (possible deadlock).'
    }
    ($stdout.Result + $stderr.Result) | Set-Content (Join-Path $testOutput 'scheduler-results.log')
    if ($process.ExitCode) { throw "Scheduler checks failed; see $testOutput/scheduler-results.log" }
    Get-Content -LiteralPath (Join-Path $testOutput 'scheduler-results.log') -Tail 3
} finally {
    $env:PATH = $previousPath
    Pop-Location
}
