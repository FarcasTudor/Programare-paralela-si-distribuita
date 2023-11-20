$NO_ROWS = $args[0]
$NO_COLUMNS = $args[1]
$OUTPUT_FILE = $args[2]
$UPPER_LIMIT = 100  # Intervalul superior pentru numerele aleatoare intre 1-100

function random_array {
    param($NO_ELEMENTS)
    $result = @()
    for ($i = 1; $i -le $NO_ELEMENTS; $i++) {
        $result += (Get-Random -Minimum 1 -Maximum $UPPER_LIMIT)
    }
    return $result
}

Set-Content -Path $OUTPUT_FILE -Value $null  # Creeaza sau rescrie fisierul de iesire

for ($i = 0; $i -lt $NO_ROWS; $i++) {
    $res = random_array $NO_COLUMNS
    Add-Content -Path $OUTPUT_FILE -Value ($res -join " ")
}