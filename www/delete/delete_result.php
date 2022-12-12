<?php

	$file_name = htmlspecialchars($_GET['filename']);
	$base_dir = realpath($_SERVER["DOCUMENT_ROOT"]);
	$file_delete = "$base_dir/delete/$file_name";
	
	if (file_exists($file_delete))
	{
		if (is_dir($file_delete))
			rmdir($file_delete);
		else
			unlink($file_delete);
		echo "<h1>Delete OK</h1>";
		echo "$file_name was correctly deleted";
	}
	else
	{
		echo "<h1>Delete KO</h1>";
		echo "$file_name wasn't deleted. File or folder doesn't exist";
	}
?> 
<br>
	<br><a href="./delete.php">Return to delete page</a><br>
	<br>
	<a href="../">Return to main page</a><br>