#
# Script to install CppUTest binary libraries in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/jgonzalezdr/cpputest/releases/download/v3.9.alpha0/cpputest-3.9.alpha0.zip'
New-Item -ItemType Directory -Force -Path "$env:CPPUTEST_HOME" | Out-Null
Invoke-Command "7z x -o$env:CPPUTEST_HOME cpputest-3.9.alpha0.zip" | Out-Null
