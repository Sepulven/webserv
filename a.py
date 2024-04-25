#!/usr/bin python3

import os
import cgi
import re
import time

content_type = {
    ".txt": "text/plain",
    ".cpp": "text/plain",
    ".hpp": "text/plain",
    ".py": "text/plain",
    ".html": "text/html",
    ".pdf": "application/pdf"
}

def POST():
    body = os.environ.get("body")
    filename_match = re.search(r'filename="(.*?)"', body)
    if filename_match:
        filename = filename_match.group(1)
    else:
        return 400

    if not filename:
        return 400

    extension = '.' + filename.split('.')[1]
    if not content_type[extension]:
        return 400

    upload_dir = "uploads/"
    entries = os.listdir(upload_dir)
    for i in entries:
        if i == filename:
            name = filename.split('.')[0]
            ext = filename.split('.')[1]
            filename = name + "_" + str(int(time.time())) + "." + ext
            break
            
    file_path = os.path.join(upload_dir, filename)
    f = open(file_path, 'w') 
    f.write(body)
    return 200

def GET():
    res = '<!DOCTYPE html>\n'
    res += '<html>\n'
    res += "<head><title>Navigation</title></head>\n"
    res += "<body>\n"
    res += "<h1>File_upload</h1>\n"

    res += '<form method="post" enctype="multipart/form-data">\n'
    res += '<label for="file-content">Choose a simple file (text only):</label><br>\n'
    res += '<input type="file" name="file-content" accept=".txt,.css,.scss,.html,.js,.svg" required>\n'
    res += '<input class="submit-button" type="submit" value="Upload">\n'
    res += '</form>\n'

    upload_dir = "uploads/"
    if not os.path.exists(upload_dir):
        os.mkdir(upload_dir)
    entries = os.listdir(upload_dir)
    if not entries:
        res += '<p>No files uploaded yet</p>'
    else:
        res += '<p>Uploaded Files:</p>'
        res += '<ul>'
        for file_name in os.listdir(upload_dir):
                res += "<li>{}</li>\n".format(file_name)
        res += '</ul>'

    res += "</body>\n"
    res += "</html>\n"

    return res

if __name__ == "__main__" :
    path = os.environ.get('path')
    method = os.environ.get('method')
    cont_type = os.environ.get('content-type')

    response = ""
    header = ""

    # read and disply error pages

    if method == "GET":
        response = GET()
        header = 'HTTP/1.1 200 OK\nContent-Type: text/html' + f'\nContent-Length: {len(response)}' + '\r\n\r\n'
    elif method == "POST":
        status = POST()
        if status == 200:
            response = "File uploaded successfully!"
            header = f'HTTP/1.1 200 OK\nContent-Type:text/plain\n' + f'Content-Length: {len(response)}' + '\r\n\r\n'
        elif status == 400:
            # response = errorPage(status)
            response == "400 Error\n"
            header = 'HTTP/1.1 400 Bad Request\nContent-Type:text/plain' + f'\nContent-Length: {len(response)}' + '\r\n\r\n'


    print(header)
    if response:
        print(response)