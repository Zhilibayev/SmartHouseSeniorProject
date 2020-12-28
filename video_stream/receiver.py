import cv2
import base64
import numpy as np

import sys
import socket

IP_SERVER = sys.argv[1]
PORT_SERVER = int(sys.argv[2])
TIMEOUT_SOCKET = 10
SIZE_PACKAGE = 4096
IMAGE_HEIGHT = 360
IMAGE_WIDTH = 640
COLOR_PIXEL = 3 

def netcat(hn, p):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((hn, p))
	res = ""
		
	while True:
		data = sock.recv(1024)
		print(len(data), data[0])
		if (not data): 
			break
		#res += data.decode()
	#print(res)
	
	print("Connection closed")
	sock.close()

def YUVtoRGB(byteArray):

        e = 640*360
        Y = byteArray[0:e]
        Y = np.reshape(Y, (360,640))

        s = e
        V = byteArray[s::2]
        V = np.repeat(V, 2, 0)
        V = np.reshape(V, (180,640))
        V = np.repeat(V, 2, 0)

        U = byteArray[s+1::2]
        U = np.repeat(U, 2, 0)
        U = np.reshape(U, (180,640))
        U = np.repeat(U, 2, 0)

        RGBMatrix = (np.dstack([Y,U,V])).astype(np.uint8)
        RGBMatrix = cv2.cvtColor(RGBMatrix, cv2.COLOR_YUV2RGB, 3)

if __name__ == '__main__':
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    connection.settimeout(TIMEOUT_SOCKET)
    connection.connect((IP_SERVER, PORT_SERVER))

    while True:
        try:
            fileDescriptor = connection.makefile(mode='rb')
            result = fileDescriptor.readline()
            fileDescriptor.close()
            result = base64.b64decode(result)
	
            #frame = np.fromstring(result, dtype=np.uint8)
            #frame_matrix = np.array(frame)

            frame_matrix = YUVtoRGB(result)#np.reshape(frame_matrix, (IMAGE_HEIGHT, IMAGE_WIDTH,COLOR_PIXEL))
            cv2.imshow('Window title', frame_matrix)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        except Exception as e:
            print ("[Error] " + str(e))

    connection.close()

