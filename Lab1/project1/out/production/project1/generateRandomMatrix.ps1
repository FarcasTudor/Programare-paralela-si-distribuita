$outputFilePath = $args[0]
$n = $args[1]  # number of rows
$m = $args[2]  # number of columns

Set-Content -Path $outputFilePath -Value $null

for ($i = 0; $i -lt $n; $i++) {
    $row = @()
    for ($j = 0; $j -lt $m; $j++) {
        $row += (Get-Random -Minimum 1 -Maximum 101)
    }
    Add-Content -Path $outputFilePath -Value ($row -join " ")
}