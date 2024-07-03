#!/usr/bin/env python3

# HTML response
# Create an infinite loop

while True:
	print("Content-type: text/html\n")
	print("<html><head><title>Sample CGI</title></head>")
	print("<body>")
	print("<h1>This is a sample CGI script</h1>")
	print("</body></html>")