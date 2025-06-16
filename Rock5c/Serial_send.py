import os, sys
sys.path.append(os.path.dirname(__file__))

import serial
import time
import requests

from Web_control import SERIAL_PORT, BAUDRATE, WEB_PORT

# mapping func
def map_val(value, inMin, inMax, outMin, outMax):
	return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin

# convert class, allow get raw data and norm to car signal
class Convert:
	def __init__(self, speed=0, steering=90):
		# set lim
		self.speed_min = 0
		self.speed_max = 100
		self.steering_min = 65
		self.steering_max = 115

		# main variables
		self.speed = speed
		self.steering = steering

		# default state
		self.dir = 'N'

	def update(self, dat):
		axes = dat.get('axes', [0, 0, 0])
		buttons = dat.get('buttons', [0] * 12)

		# Speed control
		if buttons[4]:
			self.speed = max(self.speed_min, self.speed - 10)
		elif buttons[5]:
			self.speed = min(self.speed_max, self.speed + 10)

		# Neutral / Parking
		if buttons[0]:
			self.dir = "N"
			self.speed = 0
			self.steering = 90
		elif any(buttons[i] for i in [1,2,3,6,7,8,10,11]):
			self.dir = "P"
			self.speed = 0
			self.steering = 90

		# Forward / Backward
		if axes[1] > 0.4:
			self.dir = "L"
		elif axes[1] < -0.4:
			self.dir = "T"

		# Steering
		if axes[0] > 0.4:
			self.steering = map_val(axes[0], 0.4, 1, 90, 65)
		elif axes[0] < -0.4:
			self.steering = map_val(abs(axes[0]), 0.4, 1, 90, 115)
		else:
			self.steering = 90

		# Stop trigger
		if buttons[9]:
			self.dir = "ST"

def serial_send(queue, stop_event):
	# open serial
	try:
		ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
		print(f"[Serial] Opened {SERIAL_PORT}@{BAUDRATE}")
	except Exception as e:
		print(f"[Serial] Error opening port: {e}")
		stop_event.set()
		return
	
	# process
	car = Convert()
	while not stop_event.is_set():
		if queue:
			state = queue.pop(0)
			try:
				car.update(state)
				cmd = "N" if car.dir == "ST" else car.dir

				packet = f"{cmd} {car.speed} {car.steering}\n"
				ser.write(packet.encode())
				print(f"[Serial] Sent: {packet.strip()}")

				if car.dir == "ST":
					stop_event.set()
					try:
						requests.get(f"http://127.0.0.1:{WEB_PORT}/shutdown")
					except Exception:
						pass
					break
			except Exception as e:
				print(f"[Serial] Error: {e}")
				stop_event.set()
		else:
			time.sleep(0.01)

	ser.close()
	print("[Serial] Closed port")	