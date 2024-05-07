# Function to print HTTP status code
print_status_code() {
    local status_code=$1
    case $status_code in
        200) echo "200 OK" ;;
        400) echo "400 Bad Request" ;;
        405) echo "405 Not Allowed" ;;
        404) echo "404 Not Found" ;;
        500) echo "500 Internal Server Error" ;;
        *) echo "Unknown Status Code: $status_code" ;;
    esac
}

# Function to execute curl and print status code
execute_curl() {
    local method=$1
    local desc=$2
    local text=$3
    
    local response=$(curl -X "$method" -s -o /dev/null -w "%{http_code}" $text)
    # local response=$(curl -X "$method" $text)

    echo -e $desc
    print_status_code "$response"
    echo
}

# GET

# execute_curl "GET" "\033[1;34mGet: existing file\033[0m" "http://localhost:8080/index.html"
# execute_curl "GET" "\033[1;34mGet: non existing file\033[0m" "http://localhost:8080/index13534.html" 
# execute_curl "GET" "\033[1;34mGet: file with query\033[0m" "http://localhost:8080/index.html?param1=value1&param2=value2"
# execute_curl "GET" "\033[1;34mGet: custom header \033[0m" "http://localhost:8080/index.html -H "Custom-Header: Value""
# execute_curl "GET" "\033[1;34mGet: credentials \033[0m" "http://localhost:8080/index.html -u username:password"
# execute_curl "GET" "\033[1;34mGet: special characters \033[0m" "http://localhost:8080/index.html?param=value&special_chars=%5E%25%24 -u username:password"
# # should return 304 Not Modified if the file wasnt modified since this date, and 200 if it has
# execute_curl "GET" "\033[1;34mGet: if-modified headed \033[0m" "http://localhost:8080/index.html -H "If-Modified-Since: Tue, 05 Jan 2024 00:00:00 GMT""
# # only indicates preference, the server ultimately decides what type of response to send
# execute_curl "GET" "\033[1;34mGet: accept header \033[0m" "http://localhost:8080/index.html -H "Accept: application/json""
# execute_curl "GET" "\033[1;34mGet: existing file CGI 1\033[0m" "http://localhost:8080/a.py"
# execute_curl "GET" "\033[1;34mGet: existing file CGI 2\033[0m" "http://localhost:8080/a.php"
# echo -e "\033[1;35m----------------------------\033[0m"

# POST

execute_curl "POST" "\033[1;34mPost: form data\033[0m" "http://localhost:8080/index.html -d "key1=value1""
execute_curl "POST" "\033[1;34mPost: json data\033[0m" "http://localhost:8080/index.html -H "Content-Type: application/json" -d '{"key1":"value1","key2":"value2"}'"
execute_curl "POST" "\033[1;34mPost: xml data\033[0m" "http://localhost:8080/index.html -H "Content-Type: application/xml" -d '<data><key1>value1</key1><key2>value2</key2></data>'"
execute_curl "POST" "\033[1;34mPost: file upload without content\033[0m" "http://localhost:8080/index.html -F "a.txt=hello\nworld""
execute_curl "POST" "\033[1;34mPost: files upload without content\033[0m" "http://localhost:8080/index.html -F "file1=@/home/mvicente/file2.txt" -F "file1=@/home/mvicente/file2.txt""
execute_curl "POST" "\033[1;34mPost: files upload with content\033[0m" "http://localhost:8080/index.html \
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
# EOF"
# execute_curl "POST" "\033[1;34mPost: CGI 1 form data\033[0m" "http://localhost:8080/a.py -d "hello world""
# execute_curl "POST" "\033[1;34mPost: CGI 1 files with content\033[0m" "http://localhost:8080/a.py -F "a.txt=helloooo""
# echo -e "\033[1;34mPost: CGI 1 not allowed content\033[0m"
# curl -X POST http://localhost:8080/a.py -H "Content-Type: appation/x-www-form-urlencoded" -d "key1=qetrerqwerq"

# # execute_curl "POST" "\033[1;34mPost: CGI 2 form data\033[0m" "http://localhost:8080/a.php -d "key1=qetrerqwerq""
# # execute_curl "POST" "\033[1;34mPost: CGI 2 files with content\033[0m" "http://localhost:8080/a.php -F "a.txt=helloooo" -F "b.txt=helloooo22222""
# echo -e "\033[1;34mPost: CGI 2 not allowed content\033[0m"
# curl -X POST http://localhost:8080/a.php -H "Content-Type: appation/x-www-form-urlencoded" -d "key1=qetrerqwerq"