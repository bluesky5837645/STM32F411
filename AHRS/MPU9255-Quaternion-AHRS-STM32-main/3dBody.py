import pygame
import math
from OpenGL.GL import *
from OpenGL.GLU import *
from pygame.locals import *

useSerial = True # set true for using serial for data transmission, false for wifi
useQuat = False   # set true for using quaternions, false for using y,p,r angles


import serial
ser = serial.Serial('COM3', 115200)



def main():
    video_flags = OPENGL | DOUBLEBUF
    pygame.init()
    screen = pygame.display.set_mode((1280, 720), video_flags)
    pygame.display.set_caption("Aldebaran Core AHRS - MPU9255 QUATERNION")
    resizewin(1280, 720)
    init()
    frames = 0
    ticks = pygame.time.get_ticks()
    pitch_list = [0]
    roll_list = [0]

    while 1:
        event = pygame.event.poll()
        
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            break

        line = ser.readline().decode('UTF-8').replace('\n', '')

        if len(line.split(',')) == 3:
            try:
                dataList = [float(line.split(',')[0]), float(line.split(',')[1]), float(line.split(',')[2])]
                print(dataList)
                yaw = 0
                pitch = dataList[1]
                roll = dataList[0]

                pitch = pitch - 0
                roll = roll - 0

                if pitch>360:
                    continue
                if roll > 360:
                    continue
                # if (abs(pitch-pitch_list[-1])<10):
                #     pitch_list.append(pitch)
                # else:
                #     pitch_list.append(pitch_list[-1])
                #
                # if (abs(roll - roll_list[-1]) < 10):
                #     roll_list.append(roll)
                # else:
                #     roll_list.append(roll_list[-1])


                draw(1,0,pitch,roll)
                pygame.display.flip()
                frames += 1
            except:
                continue

        # print("fps: %d" % ((frames*1000)/(pygame.time.get_ticks()-ticks)))
    if(useSerial):
        ser.close()


def resizewin(width, height):
    """
    For resizing window
    """
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()


def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def draw(w, nx, ny, nz):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glTranslatef(0, 0.0, -7.0)

    drawText((-2.6, 1.8, 2), "Aldebaran Core AHRS", 18)
    drawText((-2.6, 1.6, 2), "MPU9250 AHRS Implementation - Using Quaternion based filter", 16)
    drawText((-2.6, -2, 2), "Press Escape to exit.", 16)

    yaw = nx
    pitch = ny
    roll = nz
    drawText((-2.6, -1.8, 2), "Yaw: %f, Pitch: %f, Roll: %f" %(yaw, pitch, roll), 16)
    glRotatef(-roll, 0.00, 0.00, 1.00)
    glRotatef(pitch, 1.00, 0.00, 0.00)
    glRotatef(yaw, 0.00, 1.00, 0.00)

    glBegin(GL_QUADS)
    glColor3f(0.0, 1.0, 0.0)
    glVertex3f(1.0, 0.2, -1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(1.0, 0.2, 1.0)

    glColor3f(1.0, 0.5, 0.0)
    glVertex3f(1.0, -0.2, 1.0)
    glVertex3f(-1.0, -0.2, 1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(1.0, -0.2, -1.0)

    glColor3f(1.0, 0.0, 0.0)
    glVertex3f(1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0, -0.2, 1.0)
    glVertex3f(1.0, -0.2, 1.0)

    glColor3f(1.0, 1.0, 0.0)
    glVertex3f(1.0, -0.2, -1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(1.0, 0.2, -1.0)

    glColor3f(0.0, 0.0, 1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(-1.0, -0.2, 1.0)

    glColor3f(1.0, 0.0, 1.0)
    glVertex3f(1.0, 0.2, -1.0)
    glVertex3f(1.0, 0.2, 1.0)
    glVertex3f(1.0, -0.2, 1.0)
    glVertex3f(1.0, -0.2, -1.0)
    glEnd()


def drawText(position, textString, size):
    font = pygame.font.SysFont("Courier", size, True)
    textSurface = font.render(textString, True, (255, 255, 255, 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

if __name__ == '__main__':
    main()