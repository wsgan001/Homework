<?php
include 'Person.php';
    $person = new Person($_POST);
    $file = 'singles.txt';
    $current = file_get_contents($file);
    $lines=  explode("\n", $current);
    $persons = array();
    $suit_persons = array();
    foreach ($lines as $line){
        $values = explode(",", $line);
        $tmp_person = new Person(null);
        $tmp_person->setInfo($values);
        array_push($persons, $tmp_person);
    }
    /*下面开始匹配能够被匹配的人*/
    foreach ($persons as $sPerson){
        if($sPerson->gender == $person->seeking_female 
                || $sPerson->gender == $person->seeking_male ){
            $score = 0;
            if($person->age > $sPerson->floor_age 
                    && $person->age > $sPerson->ceil_age){
                $score +=1;
            }
            if($person->favoriteos === $sPerson->favoriteos){
                $score +=2;
            }
            for($index = 0; $index < 4; $index++){
                if(substr($person->personality, $index,1) === substr($sPerson->personality, $index,1)){
                    $score+=1;
                }
            }
            
            if($score >=3 ){
                $sPerson->rating = $score;
                array_push($suit_persons, $sPerson);
            }
            
        }else{
            continue;
        }
    }
?>

<!DOCTYPE html>
<html>
	<head>
		<title>NerdLuv</title>
		<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
		<link href="heart.gif" type="image/gif" rel="shortcut icon" />
		<link href="nerdluv.css" type="text/css" rel="stylesheet" />
	</head>

	<body>
		<div id="main">
			<div id="bannerarea">
				<img src="nerdluv.png" alt="banner logo" /> <br />
				where meek geeks meet
			</div>
	
			<div id="matches">
				<h1>Matches for Marty Stepp</h1>
<?php
foreach ($suit_persons as $sPerson){
    $picpath = strtolower($sPerson->name).".jpg";
    $picpath = str_replace(" ","_", $picpath);
    if(!file_exists("images/".$picpath)){
        $picpath = "default_user.jpg";
    }
print <<< EOT
                                <div class="match">
					<p class="name">
						<img src="images/$picpath" alt="$sPerson->name" />
						$sPerson->name 
					</p>

					<p class="info">
						<strong>gender:</strong>  $sPerson->gender <br />
						<strong>age:</strong>     $sPerson->age <br />
						<strong>type:</strong>    $sPerson->personality <br />
						<strong>OS:</strong>      $sPerson->favoriteos <br />
						<strong>rating:</strong>  $sPerson->rating
					</p>
				</div>
EOT;
}
?>
				
			</div>
		</div>
		
		<p>
			Results and page (C) Copyright 2009 NerdLuv Inc.
		</p>

		<div id="w3c">
			<a href="http://validator.w3.org/check/referer">
				<img src="http://www.w3.org/Icons/valid-xhtml11" alt="Valid XHTML 1.1" /></a>
			<a href="http://jigsaw.w3.org/css-validator/check/referer">
				<img src="http://jigsaw.w3.org/css-validator/images/vcss" alt="Valid CSS" /></a>
		</div>
	</body>
</html>
<?php
     $current.=("\n".$person->info());
     file_put_contents($file, $current);
?>