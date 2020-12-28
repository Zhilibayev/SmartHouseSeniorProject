
import numpy as np
import cv2
import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 5005

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

while(True):
    ret, frame = cap.read()
    
    cv2.imshow('frame', frame)
	
    sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    d = frame.flatten ()
    s = d.tostring ()
            
    for i in range(100):
        sock.sendto (s[i*9216:(i+1)*9216],(UDP_IP, UDP_PORT))

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
