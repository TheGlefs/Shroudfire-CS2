param(
    [Parameter(Mandatory = $true)]
    [string]$GamePath,
    [string]$AddonName = 'shroudfire'
)

$ErrorActionPreference = 'Stop'
if ($AddonName -notmatch '^[A-Za-z0-9_-]+$') {
    throw 'AddonName must contain only letters, numbers, underscores, or hyphens.'
}

$project = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$stage = Join-Path $project 'sound-pack'
$gameInstallation = (Resolve-Path -LiteralPath $GamePath).Path
$content = Join-Path $gameInstallation "content\csgo_addons\$AddonName"
$sourceSounds = Join-Path $stage 'sounds\shroudfire'
$sourceEvents = Join-Path $stage 'soundevents\soundevents_addon.vsndevts'
$sourceResourceManifest = Join-Path $stage 'resourcemanifests\shroudfire_addon_resources.vrman'
$sourceMap = Join-Path $stage 'maps\shroudfire_launch.vmap'
$sourceMaterials = Join-Path $stage 'materials\shroudfire'
$sourceStacks = Join-Path $stage 'soundstacks'

if (-not (Test-Path -LiteralPath $content -PathType Container)) {
    throw "Create the addon in CS2 Workshop Tools first: $content"
}
$resolvedContent = (Resolve-Path -LiteralPath $content).Path
if (-not $resolvedContent.StartsWith($gameInstallation.TrimEnd('\') + '\',
    [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Addon outside the selected installation: $resolvedContent"
}
if (-not (Test-Path -LiteralPath $sourceEvents -PathType Leaf)) {
    throw 'Missing sound events. Run python tools/generate_sound_pack.py first.'
}
if (-not (Test-Path -LiteralPath $sourceResourceManifest -PathType Leaf)) {
    throw 'Missing addon resource manifest in sound-pack/resourcemanifests.'
}
if (-not (Test-Path -LiteralPath $sourceMap -PathType Leaf)) {
    throw 'Missing instruction map in sound-pack/maps.'
}
if (-not (Test-Path -LiteralPath $sourceMaterials -PathType Container)) {
    throw 'Missing instruction map materials in sound-pack/materials/shroudfire.'
}
foreach ($name in 'shroudfire_movement.vsndstck', 'soundstacks_manifest.vrman') {
    if (-not (Test-Path -LiteralPath (Join-Path $sourceStacks $name) -PathType Leaf)) {
        throw "Missing local movement sound asset: soundstacks/$name"
    }
}
$clips = @(Get-ChildItem -LiteralPath $sourceSounds -Filter '*.mp3' -File)
if ($clips.Count -eq 0) { throw 'No source MP3 files found.' }

$destinationSounds = Join-Path $resolvedContent 'sounds\shroudfire'
$destinationEvents = Join-Path $resolvedContent 'soundevents\soundevents_addon.vsndevts'
$destinationResourceManifest = Join-Path $resolvedContent 'resourcemanifests\shroudfire_addon_resources.vrman'
$destinationMap = Join-Path $resolvedContent 'maps\shroudfire_launch.vmap'
$destinationMaterials = Join-Path $resolvedContent 'materials\shroudfire'
$destinationStacks = Join-Path $resolvedContent 'soundstacks'
foreach ($folder in @($destinationSounds, (Split-Path -Parent $destinationEvents),
    (Split-Path -Parent $destinationResourceManifest), (Split-Path -Parent $destinationMap),
    $destinationMaterials, $destinationStacks)) {
    New-Item -ItemType Directory -Path $folder -Force | Out-Null
    $resolvedFolder = (Resolve-Path -LiteralPath $folder).Path
    if (-not $resolvedFolder.StartsWith($resolvedContent + '\',
        [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Destination outside addon: $resolvedFolder"
    }
}
$clips | Copy-Item -Destination $destinationSounds -Force
Copy-Item -LiteralPath $sourceEvents -Destination $destinationEvents -Force
Copy-Item -LiteralPath $sourceResourceManifest -Destination $destinationResourceManifest -Force
Copy-Item -LiteralPath $sourceMap -Destination $destinationMap -Force
Get-ChildItem -LiteralPath $sourceMaterials -File | Copy-Item -Destination $destinationMaterials -Force
Get-ChildItem -LiteralPath $sourceStacks -File | Copy-Item -Destination $destinationStacks -Force

Write-Output "Copied $($clips.Count) voice sources, sound events, resource manifest, instruction map, and materials to $resolvedContent"
Write-Output 'Compile the resource manifest, soundstacks and their manifest, sound events, sounds, instruction_logo.vtex, materials, and shroudfire_launch map in Workshop Tools, then publish or update the Workshop item.'
