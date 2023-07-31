$filePath = "$env:AppData\LTspice.ini"
$line1 = "[Colors]"
$line2 = "[Colors1]"

$content = Get-Content -Path $filePath
$line1Found = $false
$line2Found = $false

for ($i = 0; $i -lt $content.Count; $i++) {
    if ($content[$i] -eq $line1 -and !$line1Found -and !$line2Found) {
        $line1Found = $true
        $content[$i] = $line2
    } elseif ($content[$i] -eq $line2 -and !$line1Found -and !$line2Found) {
        $line2Found = $true
        $content[$i] = $line1
    } elseif ($content[$i] -eq $line1 -and $line2Found) {
        $line2Found = $false
        $content[$i] = $line2
    } elseif ($content[$i] -eq $line2 -and $line1Found) {
        $line1Found = $false
        $content[$i] = $line1
    }
}

$content | Set-Content -Path $filePath

for ($i = 1; $i -le 100; $i++ )
{
Write-Progress -Activity "Cambiando el esquema de colores" -Status "$i% Complete:" -PercentComplete $i
Start-Sleep -Milliseconds 5
}