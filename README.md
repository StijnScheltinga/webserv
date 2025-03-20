# Webserv – A Custom HTTP Web Server from Scratch
The Webserv project is a challenging system programming assignment where students build a fully functional HTTP web server from the ground up in C++ (or C). Inspired by real-world web servers like NGINX and Apache, the goal is to understand the inner workings of web servers and network communication.

Key Features:
- Handles HTTP requests and responses (GET, POST, DELETE, etc.).
- Supports CGI (Common Gateway Interface) for dynamic content (e.g., running PHP scripts).
- Implements error handling, timeouts, and request limits.
- Supports configuration files, allowing users to define server behavior.
- Manages multiple clients concurrently using non-blocking I/O (e.g., select(), poll(), or epoll()).

By completing this project, students gain deep knowledge of networking, sockets, and HTTP protocols, preparing them for backend development, DevOps, and system-level programming. It’s one of the most complex and rewarding projects in the 42 curriculum!

## usage

Simply clone the project and run make in the project root.  
Then run the server providing the example config file.

![image](https://github.com/user-attachments/assets/f77f704e-517c-4aaf-8980-9531aca7d1d1)
