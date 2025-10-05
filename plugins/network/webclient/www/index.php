<?php
	
	echo "Tellusim::WebServer\n";
	
	echo "Agent: " . $_SERVER["HTTP_USER_AGENT"] . "\n";
	
	echo "Scheme: " . $_SERVER['REQUEST_SCHEME'] . "\n";
	
	echo "Method: " . $_SERVER['REQUEST_METHOD'] . "\n";
	
	if($_SERVER["REQUEST_METHOD"] == "POST") {
		echo "Name: " . $_POST["name"] . "\n";
		echo "Data: " . $_POST["data"] . "\n";
	}
?>
