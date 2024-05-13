<?php

$response = "";
$type = "";

if ($_SERVER['method'] === 'GET') {
    $items = "";
    $entries = "";
    if (count(scandir("server_uploaded_files/")) === 2)
        $items .= '<p>No files uploaded yet</p>';
    else
    {
        $entries = scandir("server_uploaded_files/");
        $items = '<p>Uploaded Files:</p><ul>';
        foreach ($entries as $file_name)
            if ($file_name !== "." && $file_name !== "..")
                $items .= "<li>{$file_name}</li>\n";
        $items .= '</ul>';
    }
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
            <label for="file-content">Choose a file:</label><br>
            <input type="file" name="file-content" required multiple>
            <input type="submit" value="Upload">
        </form>
    <?= $items ?>
    </body>
    </html>
    <?php
    $response = ob_get_clean(); // Get the captured HTML content and clear the buffer
    $status = "200 OK";
    $type = "html";
} elseif ($_SERVER['method'] === 'POST') {
    $raw_body = file_get_contents('php://stdin');

    // get boundary of post 
    $body = $_SERVER['body'];
    $request = $_SERVER['request'];

    // $ctype = $_SERVER['Content-Type'];

    $post_type = "";
    $post_type = strpos($request, "Content-Type: ");
    if ($post_type == "")
        $status = 400;
    else if (substr($request, $post_type + 14, 33) == "application/x-www-form-urlencoded")
    {
        $status = 200;
        $response = "Data uploaded successfully!\n";
        $type = "plain";
    }
    else if (substr($request, $post_type + 14, 19) != "multipart/form-data")
        $status = 400;
    else
    {
        // application/vnd.openxmlformats-officedocument.wordprocessingml.document
        $end = strpos($body, "\r\n");
        $boundary = "";
        $boundary = substr($body, 0, $end);

        // get n bodies of files
        $parts = explode("$boundary", $raw_body);
        array_pop($parts); // erases last element "--"
    
        if (!is_dir("server_uploaded_files/"))
            mkdir("server_uploaded_files/", 0777, true); // Creates the directory recursively
    
        // loop to create each file
        $count = 0;
        foreach ($parts as $part) {
            $entries = scandir("server_uploaded_files/"); // get files present int the 'server_uploaded_files' directory
            if (empty($part))
                continue;
            $filename = '';
            if (preg_match('/filename="([^"]+)"/', $part, $matches) || preg_match('/name="([^"]+)"/', $part, $matches)) { // get file name
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
                    // $milliseconds = intval($seconds * 1000) + intval($microseconds * 1000);
                    $seconds = intval($seconds) + intval($microseconds);
                    
                    if ($count != 0)
                        $filename = $name . "_" . strval($seconds) . "_" . $count . "." . $ext;
                    else
                        $filename = $name . "_" . strval($seconds) . "." . $ext;
                    $count = $count + 1;
                    break;
                }
            }
            
            $file_to_create = fopen("server_uploaded_files/" . $filename, "w"); // create file
            $start = strpos($part, "\r\n\r\n") + 4;
            $content = substr($part, $start); // get content from uploaded file
    
            fwrite($file_to_create, $content); // write content
            fclose($file_to_create);
        }
        $response = "File uploaded successfully!\n";
        $type = "plain";
        $status = 200;
    }
}

if ($status == 400)
{
    // print("check 400\n");
    // $file_to_create = fopen("error/" . $status . ".html", "r"); // read file
    $response = file_get_contents("error/" . $status . ".html");
    // $response = "Bad Request\n";
    $type = "html";
}

$contentLength = strlen($response);

echo "HTTP/1.1 $status\r\n";
echo "Content-Type: text/$type\r\n";
echo "Content-Length: $contentLength\r\n\r\n";
echo $response;

?>