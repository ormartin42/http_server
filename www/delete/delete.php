<head>
            <title>Delete</title>
        </head>
        <body>
            <h1>Delete file</h1>
			You can either:<br>
			1) Chose to delete the preexisting deletable_files or deletale_folder<br>
			2) Create the file or folder you want to delete in the folder /delete<br>
			Now write its name below, and then submit !<br><br>
            <div class="delete">
                <form action="delete_result.php" method="get" enctype="multipart/form-data">
                    <input type="text" name="filename">
                    <p><button class="styled" type="submit">Delete</button></p>
                </form>
            </div>
			<br><a href="../">Return to main page</a><br>
	<br>
</body>