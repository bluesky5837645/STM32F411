# import something package
from SerialObject import SerialObject
from HandTrackingModule import handDetector
import cv2
import socket
import math

# camera setting
cap = cv2.VideoCapture(0)
cap.set(3, 640)
cap.set(4, 480)

# socket address define
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
servAddressPort = ('192.168.1.111', 5052)

# uart serial define
STM32 = SerialObject(portNo='com3', baudRate=115200)

# Some Variable
No_hands = 0

# Detector define
detector = handDetector()

yaw_sensitivity = 0.3
scale_sensitivity = 1
last_data = [0, 0, 0, 0]

while True:
    # Read the image
    success, img = cap.read()

    # get the uart data
    uart_data = STM32.getData()

    # detect the hand
    No_hands = detector.findHands(img)
    detector.findPosition(img)
    detector.ALLfingerup()
    yaw = detector.graspAndRotate("RightHand",img,yaw_sensitivity)
    detector.gestureCommad()
    length = detector.zoomInorOut(img,detector.Command == 2)
    # make the command
    if detector.Command ==0:
        data = [0,0,0,0]
        cv2.imshow("Image", cv2.flip(img, 1))
        cv2.waitKey(1)

    elif detector.Command == 1 and len(uart_data) == 3:
        uart_data[0] = math.floor(uart_data[0] * 10) / 10
        uart_data[1] = math.floor(uart_data[1] * 10) / 10
        uart_data[2] = math.floor(uart_data[2] * 10) / 10
        cv2.imshow("Image", cv2.flip(img, 1))
        cv2.waitKey(1)
        flag = detector.uart_check(uart_data,last_data)
        last_data = uart_data
        if not flag:
            data = [1,uart_data[0],uart_data[1],yaw]
            # send the data
            print(data)
            data = str.encode(str(data))
            sock.sendto(data, servAddressPort)

        else:
            print(1)

    elif detector.Command == 2 and length is not None:
        cv2.imshow("Image", cv2.flip(img, 1))
        cv2.waitKey(1)
        data = [2, (length) * scale_sensitivity, 0, 0]
        # send the data
        print(data)
        data = str.encode(str(data))
        sock.sendto(data, servAddressPort)

    else:
        data = [0, 0, 0, 0]
        cv2.imshow("Image", cv2.flip(img, 1))
        cv2.waitKey(1)







    # send the data
    # if lmList:
    #     handroot = lmList[0][1:]
    # if len(uart_data) == 3:
    #     data = str.encode(str(uart_data))
    #     print(uart_data)
    #     sock.sendto(data, servAddressPort)

