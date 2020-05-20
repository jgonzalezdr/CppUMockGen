#
# Script to install CppUTest binary libraries in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/jgonzalezdr/cpputest/releases/download/v3.9.alpha0/cpputest-3.9.alpha0.txz'
New-Item -ItemType Directory -Force -Path "$env:CPPUTEST_HOME" | Out-Null
Invoke-Command "tar -Jxvf cpputest-3.9.alpha0.txz -C $env:CPPUTEST_HOME" | Out-Null
