$param1 = $args[0] # Nume fisier java
$param2 = $args[1] # Number of rows
$param3 = $args[2] # Number of columns
$param4 = $args[3] # Convolution size
$param5 = $args[4] # No of threads
$param6 = $args[5] # No of runs
$param7 = $args[6] # tip rulare (0 - secvential, 1 - paralel, 2 - ambele)

$suma = 0

.\generateRandomMatrix "data.txt" $param2 $param3
.\generateRandomMatrix "convolutionData.txt" $param4 $param4

for ($i = 0; $i -lt $param6; $i++){
    Write-Host "Rulare" ($i+1)
    $a = java $param1 $param2 $param3 $param4 $param5 $param7 # rulare class java
    foreach ($line in $a) {
        if( $param7 -eq 0) {
            Write-Host "Sequence time: " $line
        } else
        {
            Write-Host "Parallel time: " $line
        }
    }
    if ($param7 -ne 2) {
        $suma += $a
    }
    else {
        $suma += $a[1]
    }

    if ($param7 -eq 2) {
        $fileA = "output1.txt"
        $fileB = "output2.txt"
        if(Compare-Object -ReferenceObject $(Get-Content $fileA) -DifferenceObject $(Get-Content $fileB))
        {"files are different"}
        Else {"files are the same"}
    }

    Write-Host ""
}
$media = $suma / $i
Write-Host "Timp de executie mediu:" $media

if ($param7 -eq 0) {
    $param5 = "secvential"
}
else {
    if($param7 -eq 1) {
        $param5 = "paralel"
    }
    else {
        $param5 = "both"
    }
}
# Creare fisier .csv
if (!(Test-Path outJ.csv)){
    New-Item outJ.csv -ItemType File
    #Scrie date in csv
    Set-Content outJ.csv 'Tip Matrice,Nr threads,Timp executie'
}

# Append
Add-Content outJ.csv "N=$param2 M=$param3 K=$param4,$($param5),$($media)"