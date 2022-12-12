<?php
	if (isset($_COOKIE["giganame"]))
	{
		echo "HELLO AGAIN GIGA ";
		echo $_COOKIE["giganame"];
	}
	else
	{
		if (isset($_GET["username"]))
		{
			setcookie("giganame", $_GET["username"], time() + 60,'/');  // 1 minute
			echo "<h1>HELLO GIGA ";
			echo $_GET["username"];
			echo "</h1><br><br>To see if cookies work, <a href=".">refresh the page</a>";

		}
		else
		{
			echo "<h1>What's your name?</h1>";
			echo "<form action=\"cookies.php\" method=\"GET\">";
			echo "<input type=\"text\" name=\"username\"><br><br>";
			echo "<p><button class=\"styled\" type=\"submit\" value=\"send\">send</button></p></form>";
		}
	}
?>