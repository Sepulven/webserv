# webserv
You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won't be working on a website. 


## Note (bind failed):
	`lsof -i :8080
	 kill [PID]`


## Note (leaks and errors):
	`valgrind --leak-check=yes --show-leak-kinds=all --undef-value-errors=yes ./webserv`

## Note (epoll performance compared to other polling APIs)
	`https://suchprogramming.com/epoll-in-3-easy-steps/`