# import some important lib
import cv2
import mediapipe as mp
import time
import math


# define a handDetector class
class handDetector():
    def __init__(self, mode=False, maxHands=2, detectionCon=0.5, trackCon=0.5):
        self.mode = mode
        self.maxHands = maxHands
        self.detectionCon = detectionCon
        self.trackCon = trackCon
        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(static_image_mode=self.mode, max_num_hands=self.maxHands,
                                        min_detection_confidence=self.detectionCon,
                                        min_tracking_confidence=self.trackCon)
        self.mpDraw = mp.solutions.drawing_utils
        self.no_hands = 0

        # https://www.geeksforgeeks.org/face-and-hand-landmarks-detection-using-python-mediapipe-opencv/
        self.tipIds = [4, 8, 12, 16, 20]
        self.length_record = [];
        self.first_len = 0
        self.Command = 0
        self.handsLift = {"RightHand":[0,0,0,0,0],"LeftHand":[0,0,0,0,0]}
        self.last_position = 0

    def findHands(self, img, draw=True):
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)   # remember to cover the color space
        self.results = self.hands.process(imgRGB)
        if self.results.multi_hand_landmarks:
            # if there are two hands, then the length of self.results.multi_hand_landmarks will be 2
            # and the handLms store of infor of the specific hand index
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                    self.mpDraw.draw_landmarks(img, handLms, self.mpHands.HAND_CONNECTIONS)

        # return how many hands that ew
        if self.results.multi_hand_landmarks is not None:
            self.no_hands = len(self.results.multi_hand_landmarks)
            return len(self.results.multi_hand_landmarks)
        else:
            self.no_hands = 0
            return 0

    def findPosition(self, img, draw=True):
        # initialization
        self.handsResult = {"LeftHand": [], "RightHand": []}

        h, w, c = img.shape
        # detect the hand node position and the handtype
        if not self.no_hands == 0:
            for hand,handLandMarks in zip(self.results.multi_handedness, self.results.multi_hand_landmarks):
                hand_pos = []
                handType = hand.classification[0].label
                for id, lm in enumerate(handLandMarks.landmark):
                        cx, cy = int((lm.x) * w), int((lm.y) * h)
                        hand_pos.append([id, cx, cy])

                if handType == "Right":
                    self.handsResult["LeftHand"] = hand_pos
                if handType == "Left":
                    self.handsResult["RightHand"] = hand_pos


    def fingerup(self,handType):

        lmList = self.handsResult[handType]
        fingers = []

        # return if the specific hand is not detected
        if len(lmList) == 0:

            return [0, 0, 0, 0, 0]

        # Thumb
        # index 1 is about the cx infor
        if handType == "RightHand":
            if lmList[self.tipIds[0]][1] > lmList[self.tipIds[0] - 1][1]:
                fingers.append(1)
            else:
                fingers.append(0)
        else:
            if lmList[self.tipIds[0]][1] < lmList[self.tipIds[0] - 1][1]:
                fingers.append(1)
            else:
                fingers.append(0)

        # 4 finger
        # index 2 is about the cy infor
        for id in range(1, 5):
            root_x = lmList[0][1]
            root_y = lmList[0][2]
            length_First = math.hypot(root_x - lmList[self.tipIds[id]][1], root_y - lmList[self.tipIds[id]][2])
            length_Second = math.hypot(root_x - lmList[self.tipIds[id] - 2][1], root_y - lmList[self.tipIds[id] - 2][2])
            if length_First > length_Second:
                fingers.append(1)
            else:
                fingers.append(0)

        # return
        return fingers

    def ALLfingerup(self):
        # detected the fingerup
        right_finger = self.fingerup("RightHand")
        left_finger = self.fingerup("LeftHand")


        self.handsLift["RightHand"] = right_finger
        self.handsLift["LeftHand"] = left_finger


    def findDistance(self, handType1, handType2, p1, p2, img, draw=True):

        lmList1 = self.handsResult[handType1]
        lmList2 = self.handsResult[handType2]

        if len(lmList1) == 0 or len(lmList2) == 0:
            return None, None

        x1, y1 = lmList1[p1][1], lmList1[p1][2]
        x2, y2 = lmList2[p2][1], lmList2[p2][2]

        cx, cy = (x1 + x2) // 2, (y1 + y2) // 2
        length = math.hypot(x2 - x1, y2 - y1)

        if draw:
            cv2.circle(img, (x1, y1), 3, (255, 0, 255), cv2.FILLED)
            cv2.circle(img, (x2, y2), 3, (255, 0, 255), cv2.FILLED)
            cv2.line(img,(x1,y1),(x2,y2),(255,0,255),3)
            cv2.circle(img,(cx,cy),3,(255,0,255),cv2.FILLED)

        return length, [x1, y1, x2, y2, cx, cy]

    def zoomInorOut(self, img,flag):
        length, _ = self.findDistance("LeftHand", "RightHand", 8, 8, img, draw=flag)
        if length is None:
            self.length_record = []
            return
        if len(self.length_record) == 0:
            self.length_record.append(length)
            self.first_len = self.length_record[0]
            self.length_record[0] = 1
        else:
            self.length_record.append(length/self.first_len)

        return self.length_record[-1]

    def graspAndRotate(self,handType, img, Sensitivity):
        handPos = self.handsResult[handType]
        h, w, c = img.shape
        if len(handPos) == 0:
            self.yaw = 0
            self.last_position = 0
        else:
            self.yaw = ((handPos[0][1]-self.last_position))*Sensitivity
            self.last_position = handPos[0][1]

        return math.floor(self.yaw*10)/10

    def gestureCommad(self):
        right_finger = self.handsLift["RightHand"]
        left_finger = self.handsLift["LeftHand"]
        right_pos = self.handsResult["RightHand"]
        left_pos = self.handsResult["LeftHand"]

        if len(right_pos) > 0 and len(left_pos) == 0 and right_finger==[1,1,1,1,1]:
            self.length_record = []
            self.Command = 1
        elif len(right_pos) > 0 and len(left_pos) > 0 and right_finger==[1,1,1,1,1]:
            self.Command = 2
        else:
            self.length_record = []
            self.Command = 0

    def uart_check(self,uart_data,last_data):
        error_x = abs(uart_data[0]-last_data[0]) > 12
        error_y = abs(uart_data[1] - last_data[1]) > 12
        return  error_x or error_y or uart_data[0]>360 or uart_data[1]>360





















