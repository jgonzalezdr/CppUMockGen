#
# Script to install LCOV in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/jgonzalezdr/lcov/releases/download/v1.11w.1/lcov-v1.11w.1.zip'
Invoke-Command "7z x -oC:\ lcov-v1.11w.1.zip" | Out-Null
