# FungiGuard :mushroom:
FungiGuard is an alias for this product. It has ability to monitor and control of mushroom farming room to produce good quality and fresh mushroom. Can be remoted manually every where and every time with Thingsboard platform. :mushroom:

## A shed consists of these devices:

- 2 Sensor Units
- 2 Actuator Units 
- 1 Automation Unit
- 1 Monitor Unit

### Each sensor units

-- consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 DHT22 for humidity measurement
- 1 MH-Z19B for CO2 measurement
- 5 DS18B20: 1 for room temperature, 4 for compose temperature
- 1 MAX485 for sending sensor data to Monitor Unit
- 2 IR Emitter for sending Air Conditioner setting 
- 1 RJ45 port for transmission interface

wiring diagrams to ESP32:

| DHT22 <=> ESP32 | DS18B20 <=> ESP32 | MH>Z19B <=> ESP32 | RS485 <=> ESP32 | IR1 <=> ESP32 | IR2 <=> ESP32 |
|  DATA <=> 26    |    DATA <=> 25    |      RX <=> 33    |    RO <=> 16    |               |               |
|                 |                   |      TX <=> 32    |    DI <=> 17    |               |               |
|   VCC <=> Vin   |     VCC <=> Vin   |     VCC <=> Vin   |   VCC <=> Vin   | VCC <=> 18    | VCC <=> 27    |
|   GND <=> GND   |     GND <=> GND   |     GND <=> GND   |   GND <=> GND   | GND <=> GND   | GND <=> GND   |

<div align="center">

| Service       |                                                                                                              Command Stack                                                                                                               |                                                                                                            Query Stack                                                                                                             | 
|---------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| Account       |          [![Account - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/account-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/account-command.yaml)          |          [![Account - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/account-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/account-query.yaml)          |
| Communication | [![Communication - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/communication-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/communication-command.yaml) | [![Communication - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/communication-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/communication-query.yaml) |
| Identity      |        [![Identity - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/identity-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/identity-command.yaml)         |        [![Identity - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/identity-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/identity-query.yaml)         |
| Order         |             [![Order - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/order-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/order-command.yaml)             |             [![Order - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/order-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/order-query.yaml)             |
| Catalog       |          [![Catalog - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/catalog-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/catalog-command.yaml)          |          [![Catalog - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/catalog-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/catalog-query.yaml)          |
| Warehouse     |       [![Warehouse - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/warehouse-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/warehouse-command.yaml)       |       [![Warehouse - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/warehouse-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/warehouse-query.yaml)       |
| Shopping Cart | [![Shopping Cart - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/shopping-cart-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/shopping-cart-command.yaml) | [![Shopping Cart - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/shopping-cart-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/shopping-cart-query.yaml) |
| Payment       |          [![Payment - Command Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/payment-command.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/payment-command.yaml)          |          [![Payment - Query Stack](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/payment-query.yaml/badge.svg)](https://github.com/AntonioFalcaoJr/EventualShop/actions/workflows/payment-query.yaml)          |

 :construction: <sub>Work in Progress (WIP)</sub>

</div>

tasks:
- read sensor data
- send sensor data to Thingsboard via http protocol
- send sensor data to Monitor Unit via MAX485
- read plant phase from Thingsboard
- read Air Conditioner setting from Thingsboard
- send Air Conditioner setting via IR protocol in pronto hex format

### Each actuator units

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 1 Motor Stepper Nema 17HS4401 
- 1 Driver A4988
- 

wiring diagrams to ESP32:

| LCD I2C <=> ESP32 | RS485 <=> ESP32 |
|     SDA <=> 21    |    RO <=> 16    |
|     SCL <=> 22    |    DI <=> 17    |
|     VCC <=> Vin   |   VCC <=> Vin   |
|     GND <=> GND   |   GND <=> GND   |

tasks:
- read incoming data from sensor unit
- display data via LCD I2C

### Each automation unit

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 Terminal block (6 pin)
- 4 channel SSR relay: 1 for misting, 1 for exhaust, 2 for blower

wiring diagrams to ESP32:

|  RELAY <=> ESP32  |
| IN CH1 <=> 32     | -- MISTING PIN
| IN CH2 <=> 33     | -- EXHAUST PIN
| IN CH3 <=> 25     | -- BLOWER1 PIN
| IN CH4 <=> 26     | -- BLOWER2 PIN
|    VCC <=> Vin    |
|    GND <=> GND    |

tasks:
- read state data from Thingsboard
- trigger relay based on state data

### Each monitor unit

consists of:
- 1 ESP32 DEVKIT V1 (CHIP ESPWROOM32) as main processor
- 2 MAX485 for reading data from sensor unit
- 2 LCD I2C for displaying data
- 2 RJ45 port for transmission interface

wiring diagrams to ESP32:

| LCD I2C <=> ESP32 | RS485 <=> ESP32 |
|     SDA <=> 21    |    RO <=> 16    |
|     SCL <=> 22    |    DI <=> 17    |
|     VCC <=> Vin   |   VCC <=> Vin   |
|     GND <=> GND   |   GND <=> GND   |

tasks:
- read incoming data from sensor unit
- display data via LCD I2C