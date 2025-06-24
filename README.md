# Radxa_Rock5c-Camera-car
### Radxa Rock 5C
You can read [here](https://radxa.com/products/rock5/5c/#documentation). This is a cheap SBC that can run AI task with 6 TOPs NPU (using rknn). You can follow the document to setup and run by your self. 
### RadCar 
This is the source code for RadCar, the basis for the RadCar autonomous vehicle. I want to build my own AI car.  
## Table of content: 
-[Features](#feature)  
-[Prepare](#prepare)  
-[Installation](#installation)  
-[Usage](#usage)  
-[Contributing](#contributing)  
-[Liense](#license)

## Feature
- Control vie web
- Camera streaming

## Prepare
### Hardware
- We design part for the car in [Car_parts_solidworks](Car_parts_solidworks). 
- The others hardware parts I use in this project: 
	- [Wheels](resource\HTB1wNsMceUXBuNjt_a0q6AysXXae.jpg_720x720q50.jpg) x4
	- [Nema 17 stepper motor](resource\hhaywngFd6&_88.jpg) x2
	- [Mount plate](resource\GGhayf_5thy.jpg) x2
	- [M3 screws 5mm](resource\aggtcn46535.jpg) x4 
	- [M3 screws 40mm](resource\yu887_Bassd.jpg) x6
	- [M4 screws 10mm](resource\8_855bbabgg.jpg) x2
### Arduino
- These Arduino module are used in this project: 
	- [Arduino Uno R3](https://store.arduino.cc/products/arduino-uno-rev3) x1
	- [CNC Shield](resource\CNCd78_hye.webp) x1
	- [A4988 stepper driver](resource\drive8876Ghy.jpg) x2
	- [Step down XL4015]()

## Installation
### Python:
Python 3.8 is recommend for this project. Both Radxa and PC.  

Create a enviroment, separately with base  
```bash
# Window
python3 -m venv RadCar
RadCar/Script/activate

# Linux
python3 -m venv RadCar
source RadCar/bin/activate
```
After that, clone the repo
```bash
git clone https://github.com/VNthcong520712/Radxa_Rock5c-Camera-car
cd 'Radxa_Rock5c-Camera-car'
```
Install package dependency: 
```bash
pip install -r requirements.txt
```
### Arduino
In arduino IDE 2.3.6 you can simply install lib by select Library Manager tab, or you also can download zip from [arduino page](https://docs.arduino.cc/libraries). Download these libs:  
- [**Servo** by Michael Margolis](https://docs.arduino.cc/libraries/cncshield)
- [**CNC Shield** by Victor Gabriel Costin](https://docs.arduino.cc/libraries/servo/)

Then, you can upload your code to arduino board.  
