# FungiGuard :mushroom:

FungiGuard is an alias for this product. It has ability to monitor and control of mushroom farming (shed) to produce good quality and fresh mushroom. Can be remoted manually every where and every time with Thingsboard platform.

## Table of Contents
- [Overview](#overview)
    - [Sensor Unit](#sensor_unit)
    - [Actuator Unit](#actuator_unit)   
    - [Automation Unit](#automation_unit)
    - [Monitor Unit](#monitor_unit)
- [Installaton](#installation)
- [Directory Structure](#directory_structure)
- [To-Do-Next](#to_do_next)

<a name="overview"></a>
# Overview 

A shed consists of these devices:

- [Sensor Unit](#sensor_unit) x 2
- [Actuator Unit](#actuator_unit) x 2
- [Automation Unit](#automation_unit) x 1
- [Monitor Unit](#monitor_unit) x 1

<a name="sensor_unit"></a>
## Sensor unit  

### :package: Contains of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 DHT22 for humidity measurement
- 1 MH-Z19B for CO2 measurement
- 5 DS18B20: 1 for room temperature, 4 for compose temperature
- 1 MAX485 for sending sensor data to Monitor Unit
- 2 IR Emitter for sending Air Conditioner setting 
- 1 RJ45 port for transmission interface

### :zap: Wiring diagrams to ESP32:

<div align="center">

| DHT22 &harr; ESP | DS18B20 &harr; ESP | MH-Z19B &harr; ESP | RS485 &harr; ESP | IR1 &harr; ESP | IR2 &harr; ESP |
|:----------------:|:------------------:|:------------------:|:----------------:|:--------------:|:--------------:|
|  DATA &harr; 26  |   DATA &harr; 25   |     RX &harr; 33   |   RO &harr; 16   |                |                |
|                  |                    |     TX &harr; 32   |   DI &harr; 17   |                |                |
|  VCC &harr; Vin  |    VCC &harr; Vin  |    VCC &harr; Vin  |  VCC &harr; Vin  | VCC &harr; 18  | VCC &harr; 27  |
|  GND &harr; GND  |    GND &harr; GND  |    GND &harr; GND  |  GND &harr; GND  | GND &harr; GND | GND &harr; GND |

</div>

### :pencil2: Tasks:
- read sensor data
- send sensor data to Thingsboard via http protocol
- send sensor data to Monitor Unit via MAX485
- read plant phase from Thingsboard
- read Air Conditioner setting from Thingsboard
- send Air Conditioner setting via IR protocol in pronto hex format

<a name="actuator_unit"></a>

## Actuator unit

### :package: Contains of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 Motor Stepper Nema 17HS4401 
- 1 Driver A4988
- 1 Step Down LM2596

### :zap: Wiring diagrams to ESP32:

<div align="center">

| LCD I2C &harr; ESP32 | RS485 &harr; ESP32 |
|:--------------------:|:------------------:|
|     SDA &harr; 21    |    RO &harr; 16    |
|     SCL &harr; 22    |    DI &harr; 17    |
|     VCC &harr; Vin   |   VCC &harr; Vin   |
|     GND &harr; GND   |   GND &harr; GND   |

</div>

### :pencil2: Tasks:
- read incoming data from Thingsboard
- act as linear actuator to open circulation

<a name="automation_unit"></a>

## Automation unit 

### :package: Contains of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 Terminal block (6 pin)
- 4 channel SSR relay: 1 for misting, 1 for exhaust, 2 for blower

### :zap: Wiring diagrams to ESP32:

<div align="center">

|  RELAY &harr; ESP32  |         USAGE        |
|:--------------------:|:--------------------:|
| IN CH1 &harr; 32     | &rarr; MISTING PIN   |
| IN CH2 &harr; 33     | &rarr; EXHAUST PIN   |
| IN CH3 &harr; 25     | &rarr; BLOWER1 PIN   |
| IN CH4 &harr; 26     | &rarr; BLOWER2 PIN   |
|    VCC &harr; Vin    |                      |
|    GND &harr; GND    |                      |

</div>

### :pencil2: Tasks:
- read state data from Thingsboard
- trigger relay based on state data

<a name="monitor_unit"></a>

## Monitor unit 

### :package: Contains of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 MAX485 for reading data from sensor unit
- 2 LCD I2C for displaying data
- 2 RJ45 port for transmission interface

### :zap: Wiring diagrams to ESP32:

<div align="center">

| LCD I2C &harr; ESP32 | RS485 &harr; ESP32 |
|:--------------------:|:------------------:|
|     SDA &harr; 21    |    RO &harr; 16    |
|     SCL &harr; 22    |    DI &harr; 17    |
|     VCC &harr; Vin   |   VCC &harr; Vin   |
|     GND &harr; GND   |   GND &harr; GND   |

</div>

### :pencil2: Tasks:
- read incoming data from sensor unit
- display data via LCD I2C

- - - -

<a name="installation"></a>
# Installation
1. Make sure `Git` is installed.
2. Navigate to the folder where you want to install.
3. Right-click, open command prompt (Open in Terminal).
4. Execute the following code:

```bash
git clone https://github.com/alazifi/FungiGuard-Firmware.git
```

- - - -

<a name="directory_structure">
# Directory Structure
</a>

```bash
project/
│
├── libraries/                          # Main projects libraries
│ ├── All Arduino Libraries/            # Required libraries and modules for projects
│ └── DataRemote/                       # Library helper for AC code
│ └── README.md                         # Informations for libraries
│
├── projects/                           # Main source codes
│ ├── IEG/                              # Codes for project
│ │ ├── BDG/                            # Codes for Pangalengan projects
│ │ │ └── shed045/                      # Pangalengan shed 45
│ │ │   ├── shed045_aktuator1/          # Codes for Actuator Unit 1 
│ │ │   ├── shed045_aktuator2/          # Codes for Actuator Unit 2
│ │ │   ├── shed045_automation1/        # Codes for Automation
│ │ │   ├── shed045_monitor1/           # Codes for Monitor Unit, data taken with RS485
│ │ │   ├── shed045_monitor1_internet/  # Codes for Monitor Unit, data taken from Thingsboard
│ │ │   ├── shed045_sensor1/            # Codes for Sensor Unit 1
│ │ │   └── shed045_sensor2/            # Codes for Sensor Unit 2
│ │ │
│ │ └── BWI/                            # Codes for Banyuwangi projects
│ │   └── shed001/                      # Banyuwangi shed 1
│ │     ├── shed001_bell1/              # Codes for Alarm Bell Unit 1
│ │     ├── shed001_bell2/              # Codes for Alarm Bell Unit 2
│ │     ├── shed001_monitor1/           # Codes for Monitor Unit 1
│ │     └── shed001_monitor2/           # Codes for Monitor Unit 2
│ │
│ └── Testing/                          # Codes for trial error
│   ├── CO2/                            # Test for CO2
│   │ ├── basic_co2_serial_hardware/    # CO2 reading in Serial2 for ESP32
│   │ ├── basic_co2_softwareserial/     # CO2 reading in Software Serial
│   │ ├── co2_and_lcd/                  # Displaying CO2 value via LCD 
│   │ └── simple_co2/                   # Check reading of CO2 using serial monitor
│   │
│   ├── DS18B20/                        # Test for DS18B20 compose temperature
│   │ ├── check_ds18b20_address/        # Obtaining addresses of DS18B20 on certain pin
│   │ └── send_ds18b20_to_tb/           # Send DS18B20 data to Thingsboard via MQTT protocol
│   │
│   ├── GPIO/                           # Test for GPIO control
│   │ └── testing_tombol/               # Test INPUT pin functionality using push button
│   │
│   ├── LCD/                            # Test for LCD I2C 20x4
│   │ └── basic_multiple_lcd/           # Display data to 2 LCD using different I2C addresses
│   │
│   ├── millis/                         # Test for millis feature for multitasking
│   │ ├── blinking_two_led/             # Basic millis: blinking two led
│   │ └── parallel_task/                # Other example: connect and send in parallel task
│   │
│   ├── motor_stepper/                  # Test for motor stepper with A4988 driver
│   │ ├── motor_position/               # Determine motor position relative to zero point
│   │ ├── motor_position_ota/           # Upload code for motor using OTA
│   │ └── motor_without_library/        # Pure code to control motor rotation + OTA
│   │
│   ├── OTA/                            # Test for OTA
│   │ ├── OTA_empty/                    # Upload empty code using OTA
│   │ └── OTA_motor/                    # Upload code for motor using OTA
│   │
│   ├── send_remote_IR/                 # Test for sending IR code to AC
│   │ ├── library2prior/                # code for AC control using old library
│   │ └── library4later/                # code for AC control using latest library
│   │
│   ├── send_to_thingsboard/            # Test for sending data to Thinsgboard
│   │ ├── HTTP/                         # communicate to Thingsboard via HTTP protocol
│   │ └── MQTT/                         # communicate to Thingsboard via MQTT protocol
│   │
│   └── serial_RS485/                   # Test for serial communication RS485
│     ├── receiver/                     # ESP32 as receiver
│     ├── transmitter/                  # ESP32 as transmitter
│     └── transmitter-software-serial/  # ESP32 as transmitter via software serial
│ 
└── README.md # General project informations
```
- - - -

<a name="to_do_next"></a>
# TO-DO-NEXT
Some task must be executed after this version are:
- [ ] Wired connection architecture for all devices with one device as gateway to WiFi network.
- [ ] Implement manual operation interface for actuator and automation unit to ease maintenance and operation (plantation).
- [ ] Enlosure component with more proper enclosure, such as durabox or metal panel box.

