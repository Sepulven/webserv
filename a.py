#!/usr/bin python3

import os
import cgi
import re
import time
import uuid

content_type = {
    ".txt": "text/plain",
    ".cpp": "text/plain",
    ".hpp": "text/plain",
    ".py": "text/plain",
    ".html": "text/html",
    ".pdf": "application/pdf"
}

def repeated_file(filename, entries):
    for i in entries: # change file name if it already exists
        if i == filename:
            name = filename.split('.')[0]
            ext = filename.split('.')[1]
            filename = name + "_" + str(int(time.time())) + "." + ext

    return filename

def POST():

    upload_dir = "uploads/"
    if not os.path.exists(upload_dir):
        os.mkdir(upload_dir)
    entries = os.listdir(upload_dir)

    body = os.environ.get("body")
    if body is None:
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
        files = body.split(boundary)
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
            filename_match = re.search(r'filename="(.*?)"', file) or re.search(r'name="(.*?)"', file)# get filename

            if filename_match:
                filename = filename_match.group(1)
            elif filename == "":
                return 400

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
            f = open(file_path, 'w')
            pos = file.find("\r\n\r\n") + 4
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
    res += "<h1>File_upload</h1>\n"

    res += '<form method="post" enctype="multipart/form-data">\n'
    res += '<label for="file-content">Choose a simple file (text only):</label><br>\n'
    res += '<input type="file" name="file-content" accept=".txt,.css,.scss,.html,.js,.svg" required multiple>\n'
    res += '<input class="submit-button" type="submit" value="Upload">\n'
    res += '</form>\n'

    upload_dir = "uploads/"
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
    path = os.environ.get('path')
    method = os.environ.get('method')
    cont_type = os.environ.get('content-type')

    response = ""
    header = ""

    if method == "GET":
        response = GET()
        header = 'HTTP/1.1 200 OK\nContent-Type: text/html' + f'\nContent-Length: {len(response)}' + '\r\n\r\n'
    elif method == "POST":
        status = POST()
        if status == 200:
            response = "Data uploaded successfully!"
            header = f'HTTP/1.1 200 OK\nContent-Type: text/plain\n' + f'Content-Length: {len(response)}' + '\r\n'
        elif status == 400:
            response = errorPage(status)
            header = 'HTTP/1.1 400 Bad Request\nContent-Type: text/html' + f'\nContent-Length: {len(response)}' + '\r\n\r\n'

    print(header)
    if response:
        print(response)