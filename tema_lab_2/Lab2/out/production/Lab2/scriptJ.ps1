$param1 = $args[0] # Nume fisier java
$param2 = $args[1] # Number of rows
$param3 = $args[2] # Number of columns
$param4 = $args[3] # Convolution size
$param5 = $args[4] # No of threads
$param6 = $args[5] # No of runs
$param7 = $args[6] # Running method (0 - secvential, 1 - paralel)

$suma = 0
$suma1 = 0
$suma2 = 0

#.\generateRandomMatrix "data.txt" $param2 $param3
#.\generateRandomMatrix "convolutionData.txt" $param4 $param4

for ($i = 0; $i -lt $param6; $i++){
    Write-Host "Rulare" ($i+1)
    $a = java $param1 $param2 $param3 $param4 $param5 $param7
    if ($param7 -eq 0)
    {
        Write-Host "Sequence time: " $a
        $suma1 += $a
    }
    else
    {
        if ($param7 -eq 1)
        {
            Write-Host "Parallel time: " $a
            $suma2 += $a
        }
        else
        {
            if ($param7 -eq 2)
            {
                Write-Host "Sequence time: " $a[0]
                Write-Host "Parallel time: " $a[1]
                $suma1 += $a[0]
                $suma2 += $a[1]
            }
        }
    }
    Write-Host ""
}

if ($param7 -eq 2) {
    $tipRulare = "both"
    $fileA = "output1.txt"
    $fileB = "output2.txt"
    if(Compare-Object -ReferenceObject $(Get-Content $fileA) -DifferenceObject $(Get-Content $fileB))
    {"files are different"}
    Else {"files are the same"}
    Write-Host "Average sequence time: " ($suma1 / [int]$param6)
    Write-Host "Average parallel time: " ($suma2 / [int]$param6)
    $suma = ($suma1 / [int]$param6) + ($suma2 / [int]$param6)
    Write-Host "Average time: " ($suma / 2)
}
else {
    if ($param7 -eq 0) {
        $tipRulare = "sequence"
        $fileA = "output1.txt"
        Write-Host "Average sequence time: " ($suma1 / [int]$param6)
    }
    else {
        $tipRulare = "parallel"
        $fileA = "output2.txt"
        Write-Host "Average parallel time: " ($suma2 / [int]$param6)
    }
}

# Creare fisier .csv
if (!(Test-Path outJ.csv)){
    New-Item outJ.csv -ItemType File
    #Scrie date in csv
    Set-Content outJ.csv 'Tip Matrice,Nr threads,Timp executie'
}

if ($param7 -eq 0)
{
    Add-Content outJ.csv "$param2 $param3 $param4,$param5 $tipRulare,$($suma1 / [int]$param6)"
}
else
{
    if ($param7 -eq 1)
    {
        Add-Content outJ.csv "$param2 $param3 $param4,$param5 $tipRulare,$($suma2 / [int]$param6)"
    }
    else
    {
        Add-Content outJ.csv "$param2 $param3 $param4,$param5 $tipRulare,$($suma1 / [int]$param6) $($suma2 / [int]$param6) $($suma / 2)"
    }
}