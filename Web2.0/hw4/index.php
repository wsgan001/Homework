<!DOCTYPE html>
<html>
	<!-- CSE 190 M Homework 4 (NerdLuv) -->
	<head>
		<title>NerdLuv</title>
		<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
		<link href="heart.gif" type="image/gif" rel="shortcut icon" />
		<link href="nerdluv.css" type="text/css" rel="stylesheet" />
                <script src="js/nerdluv.js"></script>
	</head>

	<body>
		<div id="main">
			<div id="bannerarea">
				<img src="nerdluv.png" alt="banner logo" /> <br />
				where meek geeks meet
			</div>
                    <div id="form">
                    <div class="form-title">
			New User Signup:
                    </div>
                        <form action="results.php" method="post" id="sign_up_form">
                    <div class="form-body">
			<div class="pair">
                            <div class="label"><strong>Name:</strong></div>
                            <div class="set"><input type="text" maxLength="16" name="usr_name" /></div>
			</div>

			<div  class="pair">
				 <div class="label"><strong>Gender:</strong></div>
                                <div class="set"><input type="radio" name="gender" value="M"/>Male
                                 <input type="radio" name="gender" value="F" checked/>Female</div>
			</div>

			<div  class="pair">
				 <div class="label"><strong>Age:</strong></div>
                                <div class="set"><input type="text" name="age" maxLength="2" id="age"/></div>
			</div>

			<div  class="pair">
				 <div class="label"><strong>Personality type:</strong></div>
                                <div class="set"><input type="text" name="personality" maxLength="4" id="personality"/>
				(<a href="http://www.humanmetrics.com/cgi-win/JTypes2.asp">Don't know your type?</a>)
			
                                </div>
                        </div>

			<div  class="pair">
				 <div class="label"><strong>Favorite OS:</strong></div>
                                <div class="set"><select name="favoriteos">
                                    <option value="Windows">Windows</option> 
                                    <option value="Mac OS X" selected>Mac OS X</option>
                                    <option value="Linux">Linux</option>
                                    <option value="Other">Other</option>
                                </select>
                                </div>
			</div>

                        
			<div id="seeking"  class="pair">
				 <div class="label"><strong>Seeking:</strong></div>
                                  <div class="set"><input type="checkbox" value="M" name="seeking_female" checked /></div><div class="set">Male</div>
                                 <div class="set"><input type="checkbox" value="F" name="seeking_male"/></div><div class="set">Female</div>
			</div>

			<div  class="pair">
				 <div class="label"><strong>Between ages:</strong></div>
                                <div class="set"><input type="text" name="floor_age" id="floor"/>and<input type="text" name="ceil_age" id="ceil"/></div>
			</div>

			<div id="signup"  class="pair">
				Sign Up
			</div>
		</div>
                        </form>
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
                </div>
	</body>
</html>