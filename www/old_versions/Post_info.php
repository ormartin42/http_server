

<!DOCTYPE html>
<html lang="en">
<head>
<title>Post test</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Lato">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<style>
body {font-family: "Lato", sans-serif}
.mySlides {display: none}
</style>
</head>
<body>

<!-- Navbar -->
<div class="w3-top">
  <div class="w3-bar w3-black w3-card">
    <a class="w3-bar-item w3-button w3-padding-large w3-hide-medium w3-hide-large w3-right" href="javascript:void(0)" onclick="myFunction()" title="Toggle Navigation Menu"><i class="fa fa-bars"></i></a>
    <a href="index.html" class="w3-bar-item w3-button w3-padding-large">GigaChadServ</a>
    <a href="get_test.html" class="w3-bar-item w3-button w3-padding-large w3-hide-small">GET</a>
    <a href="post_test.html" class="w3-bar-item w3-button w3-padding-large w3-hide-small">POST</a>
    <a href="delete_test.html" class="w3-bar-item w3-button w3-padding-large w3-hide-small">DELETE</a>
    <div class="w3-dropdown-hover w3-hide-small">
      <button class="w3-padding-large w3-button" title="More">MORE <i class="fa fa-caret-down"></i></button>     
      <div class="w3-dropdown-content w3-bar-block w3-card-4">
        <a href="/old_html/index.html" class="w3-bar-item w3-button">autoindex page</a>
        <a href="get.php" class="w3-bar-item w3-button">Get via cgi-php</a>
        <a href="post.php" class="w3-bar-item w3-button">Post via cgi-php</a>
      </div>
    </div>
    <a href="javascript:void(0)" class="w3-padding-large w3-hover-red w3-hide-small w3-right"><i class="fa fa-search"></i></a>
  </div>
</div>


<div class="w3-container w3-content w3-center w3-padding-64" style="max-width:800px" id="band">
    <h2 class="w3-wide">POST via php-cgi</h2>
    <p class="w3-opacity"><i>We are showing you that we are managing the POST request by the php-cgi</i></p>
        <p>Dans le formulaire précédent, vous avez fourni les
        informations suivantes :</p>

        <ol>
          <li> <?php echo 'NAME: '.$_REQUEST['Name']; ?></li>
          <li> <?php echo 'MAIL: '.$_REQUEST['Email']; ?></li>
          <li> <?php echo 'MESSAGE: '.$_REQUEST['Message']; ?></li>
          <!-- Afficher l'image -->
        </ol>
</div>

<!-- End Page Content -->
</div>

<!-- Footer -->
<footer class="w3-container w3-padding-64 w3-center w3-opacity w3-light-grey w3-xlarge">
  <i class="fa fa-facebook-official w3-hover-opacity"></i>
  <i class="fa fa-instagram w3-hover-opacity"></i>
  <i class="fa fa-snapchat w3-hover-opacity"></i>
  <i class="fa fa-pinterest-p w3-hover-opacity"></i>
  <i class="fa fa-twitter w3-hover-opacity"></i>
  <i class="fa fa-linkedin w3-hover-opacity"></i>
  <p class="w3-medium">Powered by ormartin, tscandol and ljan</a></p>
</footer>

</body>
</html>