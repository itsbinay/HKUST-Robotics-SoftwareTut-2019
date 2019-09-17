# Environment Setup and Program Flashing Guide

## Step 1: Setting Up CubeIDE
1. Download and install STM32CubeIDE from here: https://www.st.com/en/development-tools/stm32cubeide.html
2. Download and unzip `F1_Internal2019` and open with STM32CubeIDE

## Step 2: Connect ST-Link to mainboard
1. Make sure the mainboard is NOT in contact with any conductive surface (e.g. metal case of your notebook, carbon fiber)
2. Locate the port `SWD` on the mainboard and connect the ST-Link as follows:


| Mainboard | ST-Link |
| --------  | --------|
| V         | 5V      |
| C         | SWCLK   |
| D         | SWDIO   |
| G         | GND     |

3. Plug the ST-Link into your computer. **UNPLUG IT IMMEDIATELY IF NO LEDS ARE ON OR FLICKERING**

## Step 1: Build and Flash the Project
1. Click on the hammer icon (shortcut: `CTRL-B`) to build the project.
2. Click on `Run` and click Debug (shortcut: `F11`).
3. Reset the mainboard to start the program.
