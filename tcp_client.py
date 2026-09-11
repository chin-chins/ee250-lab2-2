"""
EE 250L Fall 2026 - Lab 2, Part 2
Python TCP client starter code.

Complete the TODOs below.

Usage:
    python3 tcp_client.py <SERVER_IP_OR_HOSTNAME> <PORT>

Example:
    python3 tcp_client.py 127.0.0.1 5000

Expected exchange:
    1. Connect to tcp_server.py
    2. Ask the user for a short message
    3. Send the message
    4. Receive and print the server reply
    5. Close the TCP connection
"""

import socket
import sys

HOST = "127.0.0.1"
PORT = 10000

def main():
    if len(sys.argv) != 3:
     print(f"Usage: python3 {sys.argv[0]} <SERVER_IP_OR_HOSTNAME> <PORT>")
     sys.exit(1)

    server_host = sys.argv[1]

    try:
        server_port = int(sys.argv[2])
    except ValueError:
        print("ERROR: PORT must be an integer.")
        sys.exit(1)

    # making IPv4 TCP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # connecting socket 
        sock.connect((HOST, PORT))

        message = input("Enter a short message: ")

        # converting message to bytes, sending over TCP connection
        bytes = message.encode('utf-8')
        sock.sendall(bytes)

        # receive 256 bytes from server, decode them as UTF-8
        response = sock.recv(256).decode('utf-8')
        print("Server replied:", response)

    finally:
        # closing socket 
        if sock:
            sock.close()


if __name__ == "__main__":
    main()
