#!/usr/bin python3

import os
import cgi
import re
import time
import uuid
import sys

def POST():
    raw_body = sys.stdin.buffer.read()

    upload_dir = "www/uploads/"
    if not os.path.exists(upload_dir):
        os.mkdir(upload_dir)
    entries = os.listdir(upload_dir)

    # body = os.environ.get("body")
    if raw_body is None:
        return 400
    request = os.environ.get("request")
    if request is None:
        return 400

    # check post type: files or data
    post_type = request.find("Content-Type: ") + 14
    if (post_type == -1):
        return 400
    elif (request[post_type:post_type + 33] == "application/x-www-form-urlencoded"):
        return 200
    elif (request[post_type:post_type + 19] != "multipart/form-data"):
        return 400
    else:
        # find boundary in header
        bound_start = request.find("boundary=")
        bound_end = request.find("\n", bound_start + 9)
        boundary = "--" + request[bound_start + 9:bound_end]
        b_len = len(boundary)

        # get n bodies of files
        files = raw_body.split(bytes(boundary, 'utf-8'))
        if files[0] == "":
            files = files[1:]

        # erase last boundary (check split problem)
        # this might bring problems if the splits works correctly
        i = b_len + 3
        f = len(files[-1])
        files[-1] = files[-1][0:f - i]

        count = 0
        for file in files: # loop to create each file
            filename = ""
            entries = os.listdir(upload_dir)
            decoded_request = file.decode('utf-8', errors='ignore') # decode the content to get the filename
            filename_match = re.search(r'filename="(.*?)"', decoded_request) or re.search(r'name="(.*?)"', decoded_request)
            if filename_match:
                filename = filename_match.group(1)
            if filename == "":
                continue

            for i in entries: # change file name if it already exists
                if i == filename:
                    name = filename.split('.')[0]
                    ext = filename.split('.')[1]
                    # filename = name + "_" + str(int(time.time())) + "." + ext
                    if (count != 0):
                        filename = f"{name}_{str(int(time.time()))}_{count}.{ext}"
                    else:
                        filename = f"{name}_{str(int(time.time()))}.{ext}"
                    count += 1
                    break
            
            file_path = os.path.join(upload_dir, filename)
            f = open(file_path, 'wb')
            pos = file.find(b'\r\n\r\n') + 4
            content = file[pos:] # get file content
            f.write(content)
        return 200
    return 400

def errorPage(status):
    filename = "error/" + f'{status}' + ".html"
    f = open(filename, 'r')
    response = f.read() # get file content
    return response

def GET():
    res = '<!DOCTYPE html>\n'
    res += '<html>\n'
    res += "<head><title>Navigation</title></head>\n"
    res += "<body>\n"
    res += "<h1>File Upload</h1>\n"

    res += '<form method="post" enctype="multipart/form-data">\n'
    res += '<label for="file-content">Choose a file:</label><br>\n'
    res += '<input type="file" name="file-content" multiple>\n'
    res += '<input class="submit-button" type="submit" value="Upload">\n'
    res += '</form>\n'

    upload_dir = "www/uploads/"
    if os.path.exists(upload_dir) and os.listdir(upload_dir):
        entries = os.listdir(upload_dir)
        res += '<p>Uploaded Files:</p>'
        res += '<ul>'
        for file_name in os.listdir(upload_dir):
                res += "<li>{}</li>\n".format(file_name)
        res += '</ul>'
    else:
        res += '<p>No files uploaded yet</p>'

    res += "</body>\n"
    res += "</html>"

    return res

if __name__ == "__main__" :
    method = os.environ.get('method')

    response = ""
    header = ""

    if method == "GET":
        response = GET()
        header = 'HTTP/1.1 200 OK\nContent-Type: text/html' + f'\nContent-Length: {len(response) + 1}' + '\r\n\r\n'
    elif method == "POST":
        status = POST()
        if status == 200:
            response = "Data uploaded successfully!"
            header = f'HTTP/1.1 200 OK\nContent-Type: text/plain\n' + f'Content-Length: {len(response) + 1}' + '\r\n'
        elif status == 400:
            response = errorPage(status)
            header = 'HTTP/1.1 400 Bad Request\nContent-Type: text/html' + f'\nContent-Length: {len(response) + 1}' + '\r\n\r\n'

    print(header)
    if response:
        print(response)