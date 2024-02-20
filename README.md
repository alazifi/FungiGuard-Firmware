# FungiGuard :mushroom:

FungiGuard is an alias for this product. It has ability to monitor and control of mushroom farming room to produce good quality and fresh mushroom. Can be remoted manually every where and every time with Thingsboard platform. :mushroom:

## A shed consists of these devices:

- [Sensor Unit](#sensor_unit) x 2
- [Actuator Unit](#actuator_unit) x 2
- [Automation Unit](#automation_unit) x 1
- [Monitor Unit](#monitor_unit) x 1

### Each sensor units  <a name="sensor_unit"></a>

-- consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 DHT22 for humidity measurement
- 1 MH-Z19B for CO2 measurement
- 5 DS18B20: 1 for room temperature, 4 for compose temperature
- 1 MAX485 for sending sensor data to Monitor Unit
- 2 IR Emitter for sending Air Conditioner setting 
- 1 RJ45 port for transmission interface

wiring diagrams to ESP32:


<div align="center">

| DHT22 &harr; ESP | DS18B20 &harr; ESP | MH-Z19B &harr; ESP | RS485 &harr; ESP | IR1 &harr; ESP | IR2 &harr; ESP |
|:----------------:|:------------------:|:------------------:|:----------------:|:--------------:|:--------------:|
|  DATA &harr; 26  |   DATA &harr; 25   |     RX &harr; 33   |   RO &harr; 16   |                |                |
|                  |                    |     TX &harr; 32   |   DI &harr; 17   |                |                |
|  VCC &harr; Vin  |    VCC &harr; Vin  |    VCC &harr; Vin  |  VCC &harr; Vin  | VCC &harr; 18  | VCC &harr; 27  |
|  GND &harr; GND  |    GND &harr; GND  |    GND &harr; GND  |  GND &harr; GND  | GND &harr; GND | GND &harr; GND |

</div>

tasks:
- read sensor data
- send sensor data to Thingsboard via http protocol
- send sensor data to Monitor Unit via MAX485
- read plant phase from Thingsboard
- read Air Conditioner setting from Thingsboard
- send Air Conditioner setting via IR protocol in pronto hex format

<a name="actuator_unit"></a>











### Each actuator units

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 Motor Stepper Nema 17HS4401 
- 1 Driver A4988
- 1 Step Down LM2596

wiring diagrams to ESP32:

<div align="center">

| LCD I2C &harr; ESP32 | RS485 &harr; ESP32 |
|:--------------------:|:------------------:|
|     SDA &harr; 21    |    RO &harr; 16    |
|     SCL &harr; 22    |    DI &harr; 17    |
|     VCC &harr; Vin   |   VCC &harr; Vin   |
|     GND &harr; GND   |   GND &harr; GND   |

</div>

tasks:
- read incoming data from sensor unit
- display data via LCD I2C

<a name="automation_unit"></a>

### Each automation unit

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 Terminal block (6 pin)
- 4 channel SSR relay: 1 for misting, 1 for exhaust, 2 for blower

wiring diagrams to ESP32:

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

tasks:
- read state data from Thingsboard
- trigger relay based on state data

<a name="monitor_unit"></a>

### Each monitor unit 

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 MAX485 for reading data from sensor unit
- 2 LCD I2C for displaying data
- 2 RJ45 port for transmission interface

wiring diagrams to ESP32:

<div align="center">

| LCD I2C &harr; ESP32 | RS485 &harr; ESP32 |
|:--------------------:|:------------------:|
|     SDA &harr; 21    |    RO &harr; 16    |
|     SCL &harr; 22    |    DI &harr; 17    |
|     VCC &harr; Vin   |   VCC &harr; Vin   |
|     GND &harr; GND   |   GND &harr; GND   |

</div>

tasks:
- read incoming data from sensor unit
- display data via LCD I2C


# Nama Proyek Anda

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Stars](https://img.shields.io/github/stars/username/repo.svg)](https://github.com/username/repo/stargazers)

Deskripsi singkat tentang proyek Anda.

## Daftar Isi

- [Demo](#demo)
- [Fitur](#fitur)
- [Instalasi](#instalasi)
- [Penggunaan](#penggunaan)
- [Kontribusi](#kontribusi)
- [Lisensi](#lisensi)

## Demo

Tampilkan demo atau tautan ke situs web proyek Anda.

## Fitur

- Fitur 1
- Fitur 2
- Fitur 3

## Instalasi

Cara menginstal proyek Anda:

```bash
git clone https://github.com/username/repo.git
cd repo
npm install
