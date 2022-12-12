<?php
$file = $_FILES["Picture_profil"];
$fileName = $_FILES["Picture_profil"]['name'];
$fileSize = $_FILES["Picture_profil"]['size'];
$fileError = $_FILES["Picture_profil"]['error'];
$fileType = $_FILES["Picture_profil"]['type'];
/*$target_dir = "uploads/";
$target_file = $target_dir . basename($_FILES["Picture_profil"]["name"]);
$uploadOk = 1;*/
//$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
// Check if image file is a actual image or fake image
/*if(isset($_POST["submit"])) {
  $check = getimagesize($_FILES["Picture_profil"]["tmp_name"]);
  if($check !== false) {
    echo "File is an image - " . $check["mime"] . ".";
    $uploadOk = 1;
  } else {
    echo "File is not an image.";
    $uploadOk = 0;
  }
}*/
?>