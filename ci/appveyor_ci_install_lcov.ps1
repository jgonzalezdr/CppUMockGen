#
# Script to install LCOV in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/jgonzalezdr/lcov/releases/download/v1.15.alpha0w/lcov-v1.15.alpha0w.zip'
New-Item -ItemType Directory -Force -Path "$env:LCOV_HOME" | Out-Null
Invoke-Command "7z x -o$env:LCOV_HOME lcov-v1.15.alpha0w.zip" | Out-Null
