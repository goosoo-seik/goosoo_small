<#
  iar_build.ps1 - Visual Studio (Makefile/NMake project) -> IAR EWARM toolchain bridge

  Runs iarbuild.exe on the IAR project (compil\EpiaLoadMonit.ewp) and rewrites
  IAR messages into the Visual Studio format so errors and warnings show up in
  the Error List and can be double-clicked to jump to the source line.

    IAR : C:\...\file.c(235) : Error[Pe020]: identifier "x" is undefined
    VS  : C:\...\file.c(235): error Pe020: identifier "x" is undefined

  Called from GooSooRectifier.vcxproj (Build / Rebuild / Clean).
  Can also be run by hand:
    powershell -ExecutionPolicy Bypass -File iar_build.ps1 -Action make -Config Release
#>
param(
  [ValidateSet('make', 'build', 'clean')]
  [string]$Action = 'make',
  [string]$Config = 'Release',
  [string]$IarDir = 'C:\Program Files (x86)\IAR Systems\Embedded Workbench ARM 4.0 Evaluation',
  [string]$Project = ''
)

if ($Project -eq '') { $Project = Join-Path $PSScriptRoot '..\compil\EpiaLoadMonit.ewp' }

$iarbuild = Join-Path $IarDir 'common\bin\iarbuild.exe'
if (-not (Test-Path $iarbuild)) {
  [Console]::Out.WriteLine("iar_build : error IAR0001: iarbuild.exe not found: $iarbuild")
  [Console]::Out.WriteLine("iar_build : error IAR0001: set IarDir in GooSooRectifier.vcxproj or the IAR_EWARM_DIR environment variable")
  exit 1
}
if (-not (Test-Path $Project)) {
  [Console]::Out.WriteLine("iar_build : error IAR0002: IAR project not found: $Project")
  exit 1
}
$Project = (Resolve-Path $Project).Path

[Console]::Out.WriteLine("IAR $Action [$Config] $Project")
[Console]::Out.WriteLine("    toolchain: $IarDir")

$script:errors = 0
$script:pending = $null      # message whose text continues on the next line

# Messages without a source location (linker XLINK eNN, tool messages) are attached to a
# real file so the Visual Studio Error List shows a usable File column:
#   linker errors (code eNN) -> linker configuration file of this IAR configuration
#   everything else          -> the IAR project file
$compilDir = Split-Path $Project -Parent
$linkerFiles = @{
  'Release'   = Join-Path $compilDir 'resource\at91SAM7X256_FLASH.xcl'
  'RAM_Debug' = Join-Path $compilDir 'resource\at91SAM7X256_RAM.xcl'
}
function Get-Origin([string]$code) {
  if (($code -match '^e\d+$') -and $linkerFiles.ContainsKey($Config) -and (Test-Path $linkerFiles[$Config])) {
    return $linkerFiles[$Config]
  }
  return $Project
}

function Write-VsLine([string]$text) {
  [Console]::Out.WriteLine($text)
}

function Convert-IarLine([string]$line) {
  # file(line) : Error[Pe020]: text
  if ($line -match '^(?<loc>.+\(\d+\))\s*:\s*(?<sev>Fatal error|Error|Warning|Remark)\[(?<code>[^\]]+)\]:\s*(?<msg>.*)$') {
    $sev = 'error'
    if ($Matches.sev -eq 'Warning' -or $Matches.sev -eq 'Remark') { $sev = 'warning' } else { $script:errors++ }
    return @{ text = "$($Matches.loc): $sev $($Matches.code): $($Matches.msg)"; open = ($Matches.msg.Trim() -eq '') }
  }
  # "file",line  Error[Pe005]: text   (compiler format without iarbuild)
  if ($line -match '^"(?<file>[^"]+)",(?<ln>\d+)\s+(?<sev>Fatal error|Error|Warning|Remark)\[(?<code>[^\]]+)\]:\s*(?<msg>.*)$') {
    $sev = 'error'
    if ($Matches.sev -eq 'Warning' -or $Matches.sev -eq 'Remark') { $sev = 'warning' } else { $script:errors++ }
    return @{ text = "$($Matches.file)($($Matches.ln)): $sev $($Matches.code): $($Matches.msg)"; open = ($Matches.msg.Trim() -eq '') }
  }
  # linker / tool messages without a source location:  Error[e46]: Undefined external ...
  if ($line -match '^\s*(?<sev>Fatal error|Error|Warning)\[(?<code>[^\]]+)\]:\s*(?<msg>.*)$') {
    $sev = 'error'
    if ($Matches.sev -eq 'Warning') { $sev = 'warning' } else { $script:errors++ }
    $code = $Matches.code
    $msg = $Matches.msg
    $origin = Get-Origin $code
    return @{ text = "$origin : $sev ${code}: $msg"; open = ($msg.Trim() -eq '') }
  }
  return $null
}

& $iarbuild $Project "-$Action" $Config 2>&1 | ForEach-Object {
  $line = "$_"
  if ($script:pending -ne $null) {
    if ($line -match '^\s+\S') {               # continuation text of the previous message
      Write-VsLine ($script:pending + $line.Trim())
      $script:pending = $null
      return
    }
    Write-VsLine $script:pending
    $script:pending = $null
  }
  $m = Convert-IarLine $line
  if ($m -eq $null) { Write-VsLine $line }
  elseif ($m.open)  { $script:pending = $m.text }
  else              { Write-VsLine $m.text }
}
$code = $LASTEXITCODE
if ($script:pending -ne $null) { Write-VsLine $script:pending }

if (($code -ne 0) -or ($script:errors -gt 0)) {
  if ($code -eq 0) { $code = 1 }
  [Console]::Out.WriteLine("IAR $Action [$Config] FAILED (exit $code)")
  exit $code
}
[Console]::Out.WriteLine("IAR $Action [$Config] OK")
exit 0
