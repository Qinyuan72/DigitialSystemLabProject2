
import serial
import time
ser = serial.Serial('com5')  # open serial port
print(ser.name)         # check which port was really used
ser.write(b'w')     # write a string
time.sleep(10)
ser.close()             # close port