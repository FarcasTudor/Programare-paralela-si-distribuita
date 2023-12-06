$fileA = "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\outputs\rankingParallel.txt"
$fileB = "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\outputs\rankingSequential.txt"
if(Compare-Object -ReferenceObject $(Get-Content $fileA) -DifferenceObject $(Get-Content $fileB))
    {"files are different"}
Else
    {"files are the same"}