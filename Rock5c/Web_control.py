import os, sys
sys.path.append(os.path.dirname(__file__))

from flask import Flask, Response, request, jsonify
import cv2
import signal
import threading
import time

import Serial_send

# set up global variable
SERIAL_PORT = "/dev/ttyUSB0" 	# replace with "COM*" for window
BAUDRATE = 9600 
CAMERA_ID = 0 					# select the first camera
WEB_PORT = 7884					# open port that stream camera and get control signal

queue = []						# control signal queue, handle multi-thread

# inintial Flask
app = Flask(__name__)

# Stream func, capture Radxa webcam and streaming to Web
def Stream():
	while True:
		success, frame = camera.read()
		if not success:
			break
		ret, buffer = cv2.imencode('.jpg', frame)
		frame = buffer.tobytes()
		yield (b'--frame\r\n'
			   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# define base response
@app.route('/')
def video():
	return Response(Stream(), mimetype='multipart/x-mixed-replace; boundary=frame')

# listen the client for control signal 
@app.route('/control', methods=['POST'])
def control():
	data = request.json
	queue.append(data)

	print(f"Received control: {data}")
	return jsonify({"status": "success"})

# shutdown programe
@app.route("/shutdown", methods=["GET"])
def shutdown():
	camera.release()
	os.kill(os.getpid(), signal.SIGTERM)

if __name__ == '__main__':
	# open camera
	camera = cv2.VideoCapture(CAMERA_ID)

	# Necessary for usb camera
	fourcc = cv2.VideoWriter_fourcc(*'MJPG')
	camera.set(cv2.CAP_PROP_FOURCC, fourcc)
	camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
	camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

	# thread, capture the stop
	stop = threading.Event()

	# start Serial
	t1 = threading.Thread(target=Serial_send.serial_send, args=(queue, stop))
	t1.start()

	# start Web
	app.run(host='0.0.0.0', port=WEB_PORT)

	try:
		# run forever while true
		while not stop.is_set():
			time.sleep(1)
		t1.join()
		shutdown()
	except KeyboardInterrupt:
		# turn of if any interrupt detected
		print("Stopping...")
		stop.set()
		t1.join()	
		shutdown()