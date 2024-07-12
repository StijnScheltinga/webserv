#!/usr/bin/env python3

import sys
import os

post_data = sys.stdin.read()

print("")
print("<html><body>")
print("<h1>POST data</h1>")
print("<h2>")
print(post_data)
print("</h2>")
print("</body></html>")


