# Package a Nexus-ready release zip.
# Usage: .\release\package.ps1 [-Version 1.0.0]

param(
	[string]$Version = "1.0.0",
	[string]$Configuration = "RelWithDebInfo"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $Root "build\release-msvc-msvc\$Configuration"
$Dll = Join-Path $BuildDir "BuildingBudgetExtender.dll"
$Ini = Join-Path $Root "BuildingBudgetExtender.ini"
$ReleaseName = "BuildingBudgetExtender_F4SE_v$Version"
$StageDir = Join-Path $Root "release\staging\$ReleaseName"
$PluginsDir = Join-Path $StageDir "Data\F4SE\Plugins"
$ZipPath = Join-Path $Root "release\$ReleaseName.zip"

if (-not (Test-Path $Dll)) {
	Write-Error "Build output not found: $Dll`nRun: cmake --build --preset release-msvc-msvc"
}

Write-Host "Packaging $ReleaseName ..."

if (Test-Path $StageDir) {
	Remove-Item -Recurse -Force $StageDir
}
New-Item -ItemType Directory -Path $PluginsDir -Force | Out-Null

Copy-Item $Dll (Join-Path $PluginsDir "BuildingBudgetExtender.dll")
Copy-Item $Ini (Join-Path $PluginsDir "BuildingBudgetExtender.ini")
Copy-Item (Join-Path $Root "release\README.txt") (Join-Path $StageDir "README.txt")

if (Test-Path $ZipPath) {
	Remove-Item -Force $ZipPath
}
Compress-Archive -Path (Join-Path $StageDir "*") -DestinationPath $ZipPath -Force

Write-Host "Created: $ZipPath"
Write-Host "Contents:"
Get-ChildItem -Recurse $StageDir | ForEach-Object { $_.FullName.Substring($StageDir.Length + 1) }
