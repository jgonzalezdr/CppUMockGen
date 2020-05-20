#
# Script to install LCOV in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/jgonzalezdr/lcov/releases/download/v1.15.alpha1w/lcov-1.15.alpha1w.zip'
New-Item -ItemType Directory -Force -Path "$env:LCOV_HOME" | Out-Null
Invoke-Command "7z x -o$env:LCOV_HOME lcov-1.15.alpha1w.zip" | Out-Null
