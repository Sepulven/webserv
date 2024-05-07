<?php

$response = "";
$type = "";

if ($_SERVER['method'] === 'GET') {
    $items = "";
    if (is_dir("uploads/") && scandir("uploads/"))
    {
        $entries = scandir("uploads/");
        $items = '<p>Uploaded Files:</p><ul>';
        foreach ($entries as $file_name)
            if ($file_name !== "." && $file_name !== "..")
                $items .= "<li>{$file_name}</li>\n";
        $items .= '</ul>';
    }
    else
        $items .= '<p>No files uploaded yet</p>';
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
    <?= $items ?>
    </body>
    </html>
    <?php
    $response = ob_get_clean(); // Get the captured HTML content and clear the buffer
    $status = "200 OK";
    $type = "html";
} elseif ($_SERVER['method'] === 'POST') {
    // get boundary of post 
    $body = $_SERVER['body'];
    $request = $_SERVER['request'];

    $end = strpos($body, "\r\n");
    $boundary = "";
    $boundary = substr($body, 0, $end);

    if ($boundary == "") // form data case, check what to do
    {
        $status = 200;
        $response = "Form data case\n";
        $type = "plain";
    }
    else
    {

        // get n bodies of files
        $parts = explode("$boundary", $body);
        array_pop($parts); // erases last element "--"
    
        if (!is_dir("uploads/"))
            mkdir("uploads/", 0777, true); // Creates the directory recursively
    
        
        // loop to create each file
        $count = 0;
        foreach ($parts as $part) {
            $entries = scandir("uploads/"); // get files present int the 'uploads' directory
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
            
            $file_to_create = fopen("uploads/" . $filename, "w"); // create file
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

$contentLength = strlen($response);

echo "HTTP/1.1 $status\r\n";
echo "Content-Type: text/$type\r\n";
echo "Content-Length: $contentLength\r\n\r\n";
echo $response;

?>