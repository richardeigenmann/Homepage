<?php
// run from the command line as php generateComputerbooks.json.php dbpassword
$dbhost = "gladius";
$dbuser = "root";
$dbpass = $argv[1];
$mysqli = new mysqli($dbhost, $dbuser, $dbpass, "richi");
if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}
$sql = "SELECT id, Author, Title, Source, Category, ImageUrl, BookUrl, ReadDate, year(ReadDate) as ReadYear from Books where Category = 'Computers'";
$sth = mysqli_query($mysqli, $sql);
$rows = array();
while($r = mysqli_fetch_assoc($sth)) {
    $rows[] = $r;
}

$myfile = fopen("computerbooks.json", "w") or die("Unable to open file!");
fwrite($myfile, json_encode($rows));
fclose($myfile);

?>
