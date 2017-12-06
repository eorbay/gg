# socket_echo_server_uds.py

import json
import redis
import socket
import struct
import sys
import os

server_address = '/tmp/uds_sock'
r = redis.StrictRedis(host="gg-tester-001.o4kv0a.0001.usw1.cache.amazonaws.com",
                      port=6379,
                      db=0)


def handle_puts(arr):
    print("Handling puts")
    for req in arr:
        with open(req["filename"], "rb") as f:
            val = f.read()
            r.set(req["object-key"], val)

def handle_gets(arr):
    print("Handling gets")
    for req in arr:
        val = r.get(req["object-key"])
        with open(req["filename"], "wb") as f:
            f.write(val)

def handle_reqs(json):
    if json["type"] == "PUT":
        handle_puts(json["requests"])
    elif json["type"] == "GET":
        handle_gets(json["requests"])
    else:
        print("Cannot handle requests!")



# Make sure the socket does not already exist
try:
    os.unlink(server_address)
except OSError:
    if os.path.exists(server_address):
        raise

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Bind the socket to the address
print('starting up on {}'.format(server_address))
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = sock.accept()
    try:
        print('connection from', client_address)

        # Receive the data in small chunks and retransmit it
        total_data = bytes()
        data_size = 0
        while True:
            data = connection.recv(16)
            print('received {!r}'.format(data))
            total_data += data
            if len(total_data) > 4 and data_size == 0:
                data_size = struct.unpack('@I', total_data[:4])[0]
                print('Reading data of size %d Already read %d' %(data_size, len(total_data)))
                if len(total_data) >= data_size + 4:
                    break
            elif data_size != 0 and len(total_data) >= data_size + 4:
                break

        try:
            json_req = json.loads(total_data[4:])
            print(json.dumps(json_req, indent=4))
            handle_reqs(json_req)
        except json.JSONDecodeError:
            print("Error!")
            break

        print('sending data back to the client')
        connection.sendall(bytes("Done!", "ascii"))
            


    finally:
        # Clean up the connection
        connection.close()
