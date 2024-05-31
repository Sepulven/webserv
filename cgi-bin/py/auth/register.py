#!/usr/bin python3
import os
import re
import time
import uuid
import sys
import csv

def append_credentials_to_csv(file_path, credentials):
    # Parse credentials string into a dictionary
    credentials_dict = dict(item.split("=") for item in credentials.split("&"))
    
    # Extract username and password
    username = credentials_dict.get("username")
    password = credentials_dict.get("password")
    
    # Append the credentials to the CSV file
    with open(file_path, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([username, password])


def GET():
    file_path = 'cgi-bin/py/auth/register.html'
    if not os.path.exists(file_path):
        return file_path + " does not exist."
    with open(file_path, 'r') as file:
        res = file.read()
    return res

def POST():
    file_path = 'cgi-bin/py/auth/db/credentials.csv'

    if not os.path.exists(file_path):
       # If the file doesn't exist, create it with a header row
       with open(file_path, 'w', newline='') as csvfile:
           writer = csv.writer(csvfile)
           writer.writerow(['Username', 'Password'])
    raw = sys.stdin.buffer.read()
    credentials = raw.decode('utf-8')
    append_credentials_to_csv(file_path, credentials)
    return 200

if __name__ == "__main__" :
    method = os.environ.get('method')

    response = ""
    header = ""
    if method == "GET":
        response = GET()
        header = 'HTTP/1.1 200 OK\nContent-Type: text/html' + f'\nContent-Length: {len(response) + 1}' + '\r\n\r\n'
    elif method == "POST":
        status = POST()
        response = "User created sucessfully!"
        header = f'HTTP/1.1 200 OK\nContent-Type: text/plain\n' + f'Content-Length: {len(response) + 1}' + '\r\n'
    print(header)
    if response:
        print(response)