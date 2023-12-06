# Define the ranges for competitor IDs for each country
$country1Range = 100..199
$country2Range = 200..299
$country3Range = 300..399
$country4Range = 400..499
$country5Range = 500..599

# Generate a random number of competitors between 80 and 100 for each country
$minCompetitors = 80
$maxCompetitors = 100

$country1CompetitorsCount = Get-Random -Minimum $minCompetitors -Maximum $maxCompetitors
$country2CompetitorsCount = Get-Random -Minimum $minCompetitors -Maximum $maxCompetitors
$country3CompetitorsCount = Get-Random -Minimum $minCompetitors -Maximum $maxCompetitors
$country4CompetitorsCount = Get-Random -Minimum $minCompetitors -Maximum $maxCompetitors
$country5CompetitorsCount = Get-Random -Minimum $minCompetitors -Maximum $maxCompetitors

# Generate sorted lists of competitor IDs for each country
$country1Competitors = $country1Range | Select-Object -First $country1CompetitorsCount
$country2Competitors = $country2Range | Select-Object -First $country2CompetitorsCount
$country3Competitors = $country3Range | Select-Object -First $country3CompetitorsCount
$country4Competitors = $country4Range | Select-Object -First $country4CompetitorsCount
$country5Competitors = $country5Range | Select-Object -First $country5CompetitorsCount

# Create data files for each problem and country
for ($i = 1; $i -le 10; $i++) {
    $country1Competitors | ForEach-Object {
        $score = Get-Random -Minimum -1 -Maximum 101  # Generate a score between -1 and 100
        "{0},$score" -f $_
    } | Out-File -FilePath "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\results\\RezultateC1_P$i.txt" -Encoding utf8

    $country2Competitors | ForEach-Object {
        $score = Get-Random -Minimum -1 -Maximum 101  # Generate a score between -1 and 100
        "{0},$score" -f $_
    } | Out-File -FilePath "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\results\\RezultateC2_P$i.txt" -Encoding utf8

    $country3Competitors | ForEach-Object {
        $score = Get-Random -Minimum -1 -Maximum 101  # Generate a score between -1 and 100
        "{0},$score" -f $_
    } | Out-File -FilePath "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\results\\RezultateC3_P$i.txt" -Encoding utf8

    $country4Competitors | ForEach-Object {
        $score = Get-Random -Minimum -1 -Maximum 101  # Generate a score between -1 and 100
        "{0},$score" -f $_
    } | Out-File -FilePath "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\results\\RezultateC4_P$i.txt" -Encoding utf8

    $country5Competitors | ForEach-Object {
        $score = Get-Random -Minimum -1 -Maximum 101  # Generate a score between -1 and 100
        "{0},$score" -f $_
    } | Out-File -FilePath "D:\ANUL_3\PPD\tema_lab_4\Lab4\src\results\\RezultateC5_P$i.txt" -Encoding utf8
}