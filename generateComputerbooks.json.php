<?php
// run from the command line as php generateComputerbooks.json.php dbpassword
$dbhost = "localhost";
$dbuser = "richi";
$dbname = "richi";
$dbpass = $argv[1];

print ("dbhost: " . $dbhost . " dbuser: " . $dbuser . " dbname: " . $dbname . " dbpass: " . $dbpass);

$db = new PDO("mysql:host=" . $dbhost . ";dbname=" . $dbname . ";charset=utf8mb4", $dbuser, $dbpass);
$stmt = $db->query("SELECT id, Author, Title, Source, Category, ImageUrl, BookUrl, ReadDate, year(ReadDate) as ReadYear from Books where Category = 'Computers'");
$results=$stmt->fetchAll(PDO::FETCH_ASSOC);
$json=json_encode($results);
$db=null;

$myfile = fopen("computerbooks.json", "w") or die("Unable to open file!");
fwrite($myfile, $json);
fclose($myfile);

?>
