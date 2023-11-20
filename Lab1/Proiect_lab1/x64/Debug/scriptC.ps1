
$param1 = $args[0] # Nume fisier exe
$param2 = $args[1] # Number of rows
$param3 = $args[2] # Number of columns
$param4 = $args[3] # Convolution size
$param5 = $args[4] # No of threads
$param6 = $args[5] # No of runs
$param7 = $args[6] # tip rulare (0 - secvential, 1 - paralel, 2 - ambele)
$param8 = $args[7] # tip alocare (0 - dinamic, 1 - static)

# Executare exe in cmd mode
.\generateRandomMatrix "data.txt" $param2 $param3
.\generateRandomMatrix "convolutionData.txt" $param4 $param4

$suma = 0
$sumaSecvential = 0
$sumaParallel = 0

for ($i = 0; $i -lt $param6; $i++) {
    Write-Host "Rulare" ($i+1)
    $a = (cmd /c .\$param1 $param2 $param3 $param4 $param5 $param7 $param8 2>&1)

    $print = "True"
    foreach ($line in $a) {
        if( $param7 -eq 0) {
            Write-Host "Sequence method: " $line
        } else {
            if( $param7 -eq 1) {
                Write-Host "Parallel method: " $line
            } else {
                if ($print -eq "True")
                {
                    Write-Host "Sequence method: " $a[0]
                    Write-Host "Parallel method: " $a[1]
                    $sumaSecvential += $a[0]
                    $sumaParallel += $a[1]
                    $print = "False"
                }
            }
        }
        if($param7 -ne 2) {
            $suma += $line
        }
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

if($param7 -eq 0 -or $param7 -eq 1) {
	$media = $suma / $i
	Write-Host "Timp de executie mediu:" $media
} else {
	$mediaSecvential = $sumaSecvential / $i
    $mediaParallel = $sumaParallel / $i
	Write-Host "Timp de executie mediu secvential:" $mediaSecvential
	Write-Host "Timp de executie mediu paralel:" $mediaParallel
}

# Creare fisier .csv
if (!(Test-Path outC.csv)){
    New-Item outC.csv -ItemType File
    Set-Content outC.csv "Tip Matrice,Tip alocare,Nr threads,Timp executie"
}


if($param8 -eq 0) {
    $param8 = "dinamic"
}
else {
    $param8 = "static"
}

# Append
if($param7 -eq 0) {
    $param5 = "secvential"
    Add-Content outC.csv "N=$param2 M=$param3 K=$param4,$param8,$param5,$media"
}
else {
    if($param7 -eq 1) {
        Add-COntent outC.csv "N=$param2 M=$param3 K=$param4,$param8,$param5,$media"
    }
    else {
        $media = $mediaSecvential + $mediaParallel
        Add-Content outC.csv "N=$param2 M=$param3 K=$param4,$param8,$param5,$media"
    }
}