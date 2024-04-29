<?php

$response = "";

if ($_SERVER['method'] === 'GET') {
    ob_start(); // Start output buffering to capture HTML output
    ?>
    <!DOCTYPE html>
    <html>
    <head>
        <title>File Upload</title>
    </head>
    <body>
        <h1>File Upload</h1>
        <form method="post" enctype="multipart/form-data">
            <label for="file-content">Choose a file (text only):</label><br>
            <input type="file" name="file-content" accept=".txt,.css,.scss,.html,.js,.svg" required multiple><br>
            <input type="submit" value="Upload">
        </form>
    </body>
    </html>
    <?php
    $response = ob_get_clean(); // Get the captured HTML content and clear the buffer
    $status = "200 OK";
} elseif ($_SERVER['method'] === 'POST') {
    // get boundary of post 
    $body = $_SERVER['body'];
    $request = $_SERVER['request'];

    $end = strpos($body, "\r\n");
    $boundary = substr($body, 0, $end);

    // get n bodies of files
    $parts = explode("$boundary", $body);
    array_pop($parts); // erases last element "--"
    print_r($parts);

    if (!is_dir("uploads/")) {
        mkdir("uploads/", 0777, true); // Creates the directory recursively
    }

    // get files present int the 'uploads' directory
    $entries = scandir("uploads/");
    print_r($entries);

    // loop to create each file
    foreach ($parts as $part) {
        if (empty($part))
            continue;
        $filename = '';
        if (preg_match('/filename="([^"]+)"/', $part, $matches)) { // get file name
            $filename = $matches[1];
        } else {
            $status = 400;
            break;
        }

        foreach ($entries as $dir_files) { // if file exists in change name with + _timestamp
            if ($filename == $dir_files) {
                $name = explode(".", $filename)[0];
                $ext = explode(".", $filename)[1];

                list($microseconds, $seconds) = explode(' ', microtime());
                $milliseconds = intval($seconds * 1000) + intval($microseconds * 1000);

                $filename = $name . "_" . strval($milliseconds) . "." . $ext;
            }
        }
        
        $file_to_create = fopen("uploads/" . $filename, "w"); // create file

        print("\npart:\n");
        print($part);
        // $start = strpos($body, "\r\n\r\n") + 4;
        $start = 0;
        $content = substr($part, $start); // get content from uploaded file

        fwrite($file_to_create, $content); // write content
        fclose($file_to_create);
    }

    $status = 200;
}
        // write content of uploaded file to the create file in the uploads directoru

$contentLength = strlen($response);

echo "HTTP/1.1 $status\r\n";
echo "Content-Type: text/html\r\n";
echo "Content-Length: $contentLength\r\n\r\n";

// Output the response
echo $response;

?>