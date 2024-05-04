# import requests

# url = 'http://127.0.0.1:8080'

# # Define the data chunks
# chunks = [
#     '7\r\nMozilla\r\n',
#     '11\r\nDeveloper Network\r\n',
#     '0\r\n\r\n'  # Final empty chunk to indicate end of message body
# ]

# # Combine chunks into a single string
# request_body = ''.join(chunks)

# # Send the request with the entire body
# headers = {
#     'Content-Type': 'text/plain',
#     'Transfer-Encoding': 'chunked'
# }

# response = requests.post(url, headers=headers, data=request_body)

# # Print response
# print(response.text)

import requests

url = 'http://127.0.0.1:8080'

# Define the data chunks
chunks = [
    '7\r\nMozilla\r\n',
    '11\r\nDeveloper Network\r\n',
    '0\r\n\r\n'  # Final empty chunk to indicate end of message body
]

# Combine chunks into a single string
request_body = ''.join(chunks)

# Send the request with the entire body
headers = {
    'Content-Type': 'text/plain',
    'Transfer-Encoding': 'chunked'
}

response = requests.post(url, headers=headers, data=request_body)

# Print response
print(response.text)
