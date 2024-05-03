#!/bin/bash

# Test GET method
# echo -e "\033[1;34mGet: existing file\033[0m"
# curl -X GET http://localhost:8080/index.html
# echo
# echo

# echo -e "\033[1;34mGET: non existing file...\033[0m"
# curl -X GET http://localhost:8080/index13534.html
# echo
# echo

# echo -e "\033[1;34mGET: file with query...\033[0m"
# curl -X GET 'http://localhost:8080/index.html?param1=value1&param2=value2'
# echo
# echo

# echo -e "\033[1;34mGET: file with custom header...\033[0m"
# curl -X GET -H "Custom-Header: Value" http://localhost:8080/index.html
# echo
# echo

# echo -e "\033[1;34mGET: file with credentials...\033[0m"
# curl -X GET -u username:password 'http://localhost:8080/index.html'
# echo
# echo

# # check if its working
# echo -e "\033[1;34mGET: file with special characters...\033[0m"
# curl -X GET -u username:password 'http://localhost:8080/index.html?param=value&special_chars=%5E%25%24'
# echo
# echo

# # should return 304 Not Modified if the file wasnt modified since this date, and 200 if it has
# echo -e "\033[1;34mGET: if-modified header...\033[0m"
# curl -X GET -H "If-Modified-Since: Tue, 01 Jan 2020 00:00:00 GMT" http://localhost:8080/index.html
# echo

# # only indicates preference, the server ultimately decides what type of response to send
# echo -e "\033[1;34mGET: accept header...\033[0m"
# curl -X GET -H "Accept: application/json" http://localhost:8080/index.html

# echo -e "\033[1;34mGET: send 10 concurrent requests...\033[0m"
# for i in {1..10}; do
#     curl -X GET http://localhost:8080/index.html &
# done

# echo -e "\033[1;35m----------------------------\033[0m"

# Test POST method
echo -e "\033[1;34mPOST: form data...\033[0m"
curl -X POST -d "key1=value1&key2=value2" http://localhost:8080/index.html
echo
echo

echo -e "\033[1;34mPOST: json data...\033[0m"
curl -X POST -H "Content-Type: application/json" -d '{"key1":"value1","key2":"value2"}' http://localhost:8080/index.html
echo
echo

echo -e "\033[1;34mPOST: xml data...\033[0m"
curl -X POST -H "Content-Type: application/xml" -d '<data><key1>value1</key1><key2>value2</key2></data>' http://localhost:8080/index.html
echo
echo

echo -e "\033[1;34mPOST: file upload...\033[0m"
curl -X POST -F "file=uploads/a.txt" http://localhost:8080/index.html
echo
echo

echo -e "\033[1;34mPOST: files upload without file content...\033[0m"
curl -X POST -F "file1=@/home/mvicente/file2.txt" -F "file1=@/home/mvicente/file2.txt" http://localhost:8080/index.html
echo
echo

echo -e "\033[1;34mPOST: files upload with file content...\033[0m"
curl -X POST http://localhost:8080/uploads \
  -H "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW" \
  --data-binary @- <<EOF
------WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain
This is the content of file1.txt
------WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="file2"; filename="file2.txt"
Content-Type: text/plain
This is the content of file2.txt
------WebKitFormBoundary7MA4YWxkTrZu0gW--
EOF
echo
echo

# # Test DELETE method
# echo -e "\033[1;34mTesting DELETE method...\033[0m"
# curl -X DELETE http://localhost:8080/tests/a.txt
# echo

# echo -e "\033[1;35m----------------------------\033[0m"