#!/usr/bin/python3
import os

print("Content-type: text/html\n\n")
print("<html>Hello world!</html>")

if 'REQUEST_METHOD' in os.environ :
    print("This is a webpage") # cgi executing
else :
    print("This is not a webpage") # local execution