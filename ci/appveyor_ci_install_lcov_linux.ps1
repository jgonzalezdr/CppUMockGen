#
# Script to install LCOV in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

Start-FileDownload 'https://github.com/linux-test-project/lcov/releases/download/v1.15/lcov-1.15.tar.gz'
New-Item -ItemType Directory -Force -Path "$env:LCOV_HOME" | Out-Null
Invoke-Command "tar -zxvf lcov-1.15.tar.gz --strip 1 -C $env:LCOV_HOME" | Out-Null

