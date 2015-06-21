<?php
//本页title
$title;
//本页rating
$rate;

//本页年份
$year;

//有多少个review
$review_count;

//目前显示多少个view
$show_view;

//本页generalView 的pic path
$generalView;



/*本程序将读取$folder下的reviews，并按照格式输出来*/
function getReviews($folder){
	

}

function getAllReviews($folder){
global $show_view; 
$files = glob($folder."/review*.txt");
$show_view = count($files);
echo "<div class=\"left\">";

for($i =1; $i <= count($files); $i++){
	$vars = file($files[$i-1]);
	$comment;
	$fresh;
	$author;
	$from;
	list($comment,$fresh,$author,$from) = $vars;
	if($i > count($files)/2){
		echo "</div><!--End of left--><div class=\"right\">";
	}
	echo "<p class=\"comment\">";
	if($fresh == "ROTTEN\n"){
		echo "<img src=\"http://222.200.185.14/public/hw3/fresh.gif\" alt=\"Rotten\" />";
	}else if($fresh == "FRESH\n"){
		echo "<img src=\"http://222.200.185.14/public/hw3/rotten.gif\" alt=\"Fresh\" />";
	}else{
		die($fresh."Fresh or Rotten");
	}
	echo "<q>".$comment."</q>";
	
	echo "</p><img src=\"http://222.200.185.14/public/hw3/critic.gif\" alt=\"Critic\" />";
	echo "<div class=\"name\">".$author." </div>";
	echo "<div class=\"publication\">".$from."</div>";
}
echo "</div>";

}
function readInfo($folder){
	global $title,$year,$rating,$review_count;
	$vars = file($folder."/info.txt");
	list($title,$year,$rating,$review_count) = $vars;
}
function printInfo($folder){
global $title,$year,$rating,$review_count;
if($rating >=60){
	$img_path =  "http://222.200.185.14/public/hw3/freshbig.png";
}else{
	$img_path =  "http://222.200.185.14/public/hw3/rottenbig.png";
}
print <<<EOT
	<div class="left-central-top">
			<img src="$img_path" alt="Rotten" />
			<div class="rating">$rating%</div> <div class="total">($review_count reviews total)</div>
		</div>
EOT;
}
/*
 * 本程序将输出General OverView的内容
 * */
function generalOverView($folder){

print <<<EOT
	<div class="right_overview">
        <div>
			<img src="$folder/generaloverview.png" alt="general overview" />
		</div>
	<dl>
EOT;
$vars = file($folder."/generaloverview.txt");
for($i = 0; $i < count($vars); $i++){
	$infos = explode(":", $vars[$i]);
	echo "<dt>".$infos[0]."</dt>";
	echo "<dd>".$infos[1]."</dd>";
}//外层循环

print <<<EOT

		</dl>
        </div><!--right ov-->
EOT;
}
/*
 * 本程序将输出整个网页的大框架
 * global $title  $banner
 *
 * */
function mainFrame(){
	$movie = $_REQUEST["film"];
	$folder = $movie;
	readInfo($folder);
	global $title,$year,$show_view,$review_count;
print <<<EOT
<!DOCTYPE html>
    <html>
	<head>
		<meta charset="utf-8" />
		<title>$title</title>
        <link rel="stylesheet" href="movie.css"/>
        <link rel="shortcut icon" href="http://222.200.185.14/public/hw3/rotten.gif" />
	</head>
	<body>
		<div  class="banner">
			<img src="http://222.200.185.14/public/hw3/banner.png" alt="Rancid Tomatoes" />
		</div>

		<h1 class="heading">$title($year)</h1>
		<div class="content_area">
EOT;
generalOverView($folder);
print <<<EOT
		
<div class="left-central">
EOT;
printInfo($title);
getAllReviews($folder);

print <<<EOT
        </div><!--End of left-central-->
        <div class="bar">(1-$show_view) of $review_count</div>
        </div><!--End of content area-->
	</body>
</html>
EOT;
}


mainFrame();