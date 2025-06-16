import pygame 
import time
import requests

# set up global variable
JOY_ID = 1								# choose JS ID that works for you
WAIT_TIME = 20
SERVER_ID = 'http://192.168.1.209:7884'	# replace with Radxa IP

# send data to webserver
def send_control(data):
	url = f"{SERVER_ID}/control"
	try: 
		response = requests.post(url, json=data)
		return response.json()
	except Exception as e:
		print(f"Error: {e}")

# read joystick with pygame
def read_joy(test = False):
	# if test, the funct will not send to web the data
	pygame.init()
	pygame.joystick.init()

	joystick_c = pygame.joystick.get_count()
	if joystick_c == 0:
		print("No gamepad detected")
		return
	else:
		print("Detected", joystick_c, "devices!")
	
	joystick = pygame.joystick.Joystick(JOY_ID)
	joystick.init()
	print(f"Gamepad detected: {joystick.get_name()}")

	prev_state = {}
	
	try:
		while True: 
			pygame.event.pump()
			
			axes = joystick.get_numaxes()
			Axes = [0]*axes
			for i in range(axes): 
				axis = joystick.get_axis(i)
				Axes[i] = round(axis, 3)

			buttons = joystick.get_numbuttons()
			Buttons = [0]*buttons
			for j in range(buttons): 
				button = joystick.get_button(j)
				Buttons[j] = button

			hats = joystick.get_numhats()
			Hats = [0]*hats
			for z in range(hats):
				hat = joystick.get_hat(z)
				Hats[z] = hat

			state = {
				'axes': Axes,
				'buttons': Buttons,
				'hats': Hats
			}

			if state != prev_state:
				if not test: send_control(state)
				print(state)
				print("\n","-"*30)

			prev_state = state
			time.sleep(WAIT_TIME/1000)
	except KeyboardInterrupt:
		print("Canceling...")
	finally: 
		joystick.quit()
		pygame.quit()

if __name__ == "__main__":
	read_joy()