'''
TeleOp Program for NSL_S2AV
@Author: Behrad Toghi, Christian Theriot
@Version: 1.1.2
#SSH added - highspeed enabled
'''

# Libraries
import math
import keyboard
import os
import time
import os
import paramiko



class vehicle(object):
    #constructor
    def __init__(self, setThrottle, setSteering):
        self.throttle = setThrottle
        self.steering = setSteering

    #getters

    #setters

    def __str__(self):
        return "Throttle = " + str(math.floor(self.throttle)) + " , " + "Steering = " +  str(math.floor(self.steering))

# def getCh():
#     import sys
#     import termios
#     import fcntl
#     import os
#     fd = sys.stdin.fileno()
#
#     oldterm = termios.tcgetattr(fd)
#     newattr = termios.tcgetattr(fd)
#     newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
#     termios.tcsetattr(fd, termios.TCSANOW, newattr)
#     oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
#     fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)
#     try:
#         while 1:
#             try:
#                 c = sys.stdin.read(1)
#                 break
#             except IOError:
#                 pass
#     finally:
#         termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
#         fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
#     return c
##############################################

def throttle_forward(teleOp, sftp):
    ttl_step = 2
    teleOp.throttle += ttl_step
    write(int(math.floor(teleOp.throttle)), 'throttle.txt', sftp)

def throttle_reverse(teleOp, sftp):
    ttl_step = 2
    teleOp.throttle -= ttl_step
    write(int(math.floor(teleOp.throttle)), 'throttle.txt', sftp)

def steer_right(teleOp, sftp):
    str_step = 1
    teleOp.steering += str_step
    write(int(math.floor(teleOp.steering)), 'steering.txt', sftp)

def steer_left(teleOp, sftp):
    str_step = 1
    teleOp.steering -= str_step
    write(int(math.floor(teleOp.steering)), 'steering.txt', sftp)

def hard_right(teleOp, sftp):
    teleOp.steering = 125
    write(int(math.floor(teleOp.steering)), 'steering.txt', sftp)

def hard_left(teleOp, sftp):
    teleOp.steering = 65
    write(int(math.floor(teleOp.steering)), 'steering.txt', sftp)

def hard_brake(teleOp, sftp):
    if 1900 > teleOp.throttle > 1650:
        goToPark( sftp)
        teleOp.throttle = 1050
    elif 1100 < teleOp.throttle <1550:
        goToPark( sftp)
        teleOp.throttle = 1950



def write (val, path, sftp):
    f = open(path, 'w')
    f.write(str(val))
    f.close()

    # paramiko.util.log_to_file('/tmp/paramiko.log')
    # paramiko.util.load_host_keys(os.path.expanduser('~/.ssh/known_hosts'))
    
    

    remote_images_path = '/home/nvidia/Documents/dev/src/py/teleOp/'
    local_path = 'C:/Users/Rodolfo/Downloads/olddev/src/py/teleOp/'
    file_remote = remote_images_path + path
    file_local = local_path + path

    sftp.put(file_local, file_remote)


def goToPark(sftp):
    ttl = 1600
    str = 90
    teleOp = vehicle(ttl, str)

    write((teleOp.throttle), 'throttle.txt', sftp)
    write((teleOp.steering), 'steering.txt', sftp)
    return teleOp

def main():

    #SSH
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(
                paramiko.AutoAddPolicy())
    #ssh.connect('192.168.0.102', username='nvidia', password='nvidia')
    ssh.connect('10.173.215.138', username='nvidia', password='nvidia')
    sftp = ssh.open_sftp()


    #Initialization
    run = True
    brake = False
    steer = False
    teleOp = goToPark(sftp)
    while run:

        if keyboard.is_pressed("w"):
            if teleOp.throttle < 1950:
                throttle_forward(teleOp, sftp)

        if keyboard.is_pressed("s"):
            if teleOp.throttle > 1050:
                throttle_reverse(teleOp, sftp)

        if keyboard.is_pressed("d"):
            if teleOp.steering < 125:
                steer_right(teleOp, sftp)

        if keyboard.is_pressed("a"):
            if teleOp.steering > 65:
                steer_left(teleOp, sftp)

        if steer and not keyboard.is_pressed("e") and not keyboard.is_pressed("q"):
            teleOp.steering = 90
            write(teleOp.steering, 'steering.txt', sftp)
            steer = False

        if keyboard.is_pressed("e"):
            hard_right(teleOp, sftp)
            steer = True

        if keyboard.is_pressed("q"):
            hard_left(teleOp, sftp)
            steer = True

        if brake and not keyboard.is_pressed("b"):
            teleOp.throttle = 1600
            write(teleOp.throttle, 'throttle.txt', sftp)
            brake = False

        if keyboard.is_pressed("b"):
            hard_brake(teleOp, sftp)
            brake = True

        if keyboard.is_pressed("p"):
            teleOp = goToPark( sftp)

        if keyboard.is_pressed("0"):
            teleOp = goToPark( sftp)
            write((teleOp.throttle), 'throttle.txt', sftp)
            run = False

        showDash(teleOp)
        time.sleep(0.005)
    sftp.close()
    ssh.close()
    print('\n **Program Terminated**')



def showDash(teleOp):

    print('''
               ****************************************************************************
               ******                    **Networked Systems Lab**                   ******
               ******                     TeleOp V.1.0.1 Running                     ******
               ******                                                                ****** 
               ******                              |W|                               ******
               ******                                                                ****** 
               ******       Hard Left: |Q|    |A|  |S|  |D|    Hard Right: |E|       ******
               ******                                                                ******
               ******                         |B|       |P|                          ******
               ******                        Brake      Park                         ****** 
               ****************************************************************************                                                               
               Odometry Info:''')
    print (teleOp)
    print('''  
               ****************************************************************************
               Press "0" to Terminate
        
               TeleOp by: Behrad Toghi, Christian Theriot - NSL
               ****************************************************************************                                                               
               ''')

if __name__ == "__main__":
    main()
