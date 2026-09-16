@echo off
setlocal

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $dir='%~dp0'; $bat='%~nx0'; Get-ChildItem -LiteralPath $dir -File | Where-Object { $_.Name -ne $bat } | ForEach-Object { $old=$_.FullName; $new=$_.Name.ToLowerInvariant(); if($old -cne (Join-Path $dir $new)){ $tmp=Join-Path $dir ('TEMP_' + [guid]::NewGuid().ToString('N')); Move-Item -LiteralPath $old -Destination $tmp; Move-Item -LiteralPath $tmp -Destination (Join-Path $dir $new); Write-Host ($_.Name + ' -> ' + $new) } }"

echo.
pause