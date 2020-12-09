import serial
import sys

ser = serial.Serial('/dev/ttyACM0', 9600)
text=sys.argv[1]
ser.write(text.encode())

ser.close()
