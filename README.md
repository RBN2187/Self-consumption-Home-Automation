# AHC - Automated Home Controller
Automating energy consumption to maximize self-consumption and minimize peak usage.
## Overview
The scope of this project is controlling outlets remotely via WiFi for energy-intensive devices such as a dishwasher, electric heater, washing machine, etc.

Every outlet will feature a "smart-plug" controlled by a central control unit. This unit will also act as a user interface to display data like current production and consumption.
## Units
### Central controller - ODIN
ODIN will be the brains of the project, acting as a hub that controls every plug in the house. All other modules
will act as slaves. ODIN will send requests to the slave modules to retrieve current consumption and production. In addition, ODIN will feature a touch screen, 
so it can act as a user interface. It can display data, and the user can make requests, for example, to give a particular device a higher priority to stay on.
### Plug smart switch - VIDAR
VIDAR will be the smart-plug between the wall socket and the energy-intensive devices.
