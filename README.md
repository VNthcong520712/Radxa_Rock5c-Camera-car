# Radxa_Rock5c-Camera-car
### Radxa Rock 5C
You can read [here](https://radxa.com/products/rock5/5c/#documentation). This is a cheap SBC that can run AI task with 6 TOPS NPU (using rknn). You can follow the document to setup and run by your self. 
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
