#!/usr/bin python3
import os
import re
import time
import uuid
import sys
import csv

def GET():
    file_path = 'cgi-bin/py/auth/login.html'
    with open(file_path, 'r') as file:
        res = file.read()
    return res

if __name__ == "__main__" :
    method = os.environ.get('method')
    body = os.environ.get('body')

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