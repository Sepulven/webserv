#!/usr/bin python3
import os
import sys
import csv
import secrets
import string

def session_exists(file_path, session_id):
    with open(file_path, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row[0] == session_id:
                return True
    return False

def add_session_to_ids(session_id):
    file_path = 'cgi-bin/py/auth/db/ids.csv'
    with open(file_path, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([session_id])

def generate_session_id(length=32):
    alphabet = string.ascii_letters + string.digits
    session_id = ''.join(secrets.choice(alphabet) for _ in range(length))
    return session_id

def credentials_exist(file_path, username, password):
    # Read the CSV file
    with open(file_path, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            # Each row is in the format [username, password]
            if row and row[0] == username and row[1] == password:
                return True
    return False

def GET():
    file_path = 'cgi-bin/py/auth/login.html'
    if not os.path.exists(file_path):
        return file_path + " does not exist."
    with open(file_path, 'r') as file:
        res = file.read()
    return res

def POST():
    file_path = 'cgi-bin/py/auth/db/credentials.csv'
    if not os.path.exists(file_path):
        return 'NO_CREDENTIALS'
    raw = sys.stdin.buffer.read()
    credentials = raw.decode('utf-8')
     # Parse credentials string into a dictionary
    credentials_dict = dict(item.split("=") for item in credentials.split("&"))
    username = credentials_dict.get("username")
    password = credentials_dict.get("password")
    if credentials_exist('cgi-bin/py/auth/db/credentials.csv', username, password):
        return 'SUCESS'
    return 'FAILURE'


if __name__ == "__main__" :
    method = os.environ.get('method')
    cookie = os.environ.get('cookie')
    response = ""
    header = ""
    if method == "GET":
        response = GET()
        header = 'HTTP/1.1 200 OK\nContent-Type: text/html' + f'\nContent-Length: {len(response) + 1}' + '\r\n\r\n'
    elif method == "POST":
        response = POST()
        if response == 'NO_CREDENTIALS':
            header = f'HTTP/1.1 302 Found\r\nLocation: /register\r\n\r\n'
        elif response == 'SUCESS':
            session_id = generate_session_id()
            if not session_exists('cgi-bin/py/auth/db/ids.csv', cookie):
                add_session_to_ids(session_id)
            header = f'HTTP/1.1 302 Found\r\nSet-Cookie: ' + session_id + '\r\nLocation: /py\r\n\r\n'
        else:
            header = f'HTTP/1.1 302 Found\r\nLocation: /login\r\n\r\n'
        response = ""

    print(header)
    if response:
        print(response)