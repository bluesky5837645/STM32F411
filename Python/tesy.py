import cv2
import time

cap = cv2.VideoCapture(1)
cTime = 0
pTime = 0

while True:
    success, img = cap.read()
    cTime = time.time()
    FPS = 1/(cTime - pTime)
    print(int(FPS))
    pTime = cTime
    if success:
        cv2.imshow("Image Display",img)
        key = cv2.waitKey(1)
        if key == ord("q"):
            cv2.destroyAllWindows()
            break





