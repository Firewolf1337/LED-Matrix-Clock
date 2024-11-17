# LED-Matrix-Clock
This is a basically stupid display which is getting all information from Home Assistant.
It can show pre configured icon and print any text send to it.
It can be setup to scroll the text and/or icon. And to any text color all from Home Assistant.
## Prerequisites:
- Home Assistant
- ESPhome Add-on installed
- [ESP32](https://amzn.eu/d/iIpKc6O)
- 32x8 [LED-Matrix](https://amzn.eu/d/cav3gaP)

## For the beginning:
- In Home Assistant open the File editor.
- Go to the homeassistant/esphome folder
- Create two new folder. "_fonts" and "_icons"
- Copy the files from the folder of the same name in this repository to the corresponding folders in the Home Assistant.
### If not already present:
- Create the secrets file
    - Go to the ESPHome Add-on
    - On the top right click on secrets
- add at least this four lines. 
    - `wifi_ssid: "MY_WIFI_NAME_WHERE_THE_ESP_SHOULD_CONNECT"`
    - `wifi_password: "THE_PASSWORD_FOR_THAT_WIFI"`
    - `ota_password: "RANDOM_32_CHARS_KEY" # like this 6dcd21a1985bb15beba465dad5b32394`
    - `api_key: "RANDOM_CREATED_KEY"`
        - To get an random API Key visit [esphome docu](https://esphome.io/components/api.html#configuration-variables) 


## Preparing the Hardware
1. Adding new device to ESPhome in Home Assistant:
    - Go to ESPhome in Home Assistant
    - Click on "+ NEW DEVICE" on the bottom right
    - Enter the name you want.
    - Skip the next step
    - Select ESP32
    - Skip the next step
    - Click on "EDIT" at the new created device

2. Enter and Preparing the code
    - Remove the shown code
    - Enter the code from the espcode.c file (open with and text editor)
        - make sure you keep the format and tabstops in the file.
    - Change the device name in line 2, to what ever the device should be called.
    - Check the Wifi section.
    - Save the Code.

3. First installation
    - Connect the ESP32 with your PC.
    - Click the "INSTALL" button on the top right.
    - Select Plug into this computer.
    - In the popup window select the COM-port where your ESP is connected (replug the ESP32 to check which is the port you need.)
        - If it is not shown, try to install the driver (most likely CP2102 if it is the ESP32 linked above)
    - Installation should start after clicking connect.
    - Wait to finish the installation.

4. Connect the matrix to the ESP32
    - Connect Matrix 5V to ESP32 5V
    - Connect Matrix GND to ESP32 GND
    - Connect Matrix DIN to ESP32 G32 (can be changed in the code)

## Preparing the Automation (entities has to be changed to match your environment)
1. To show the Time just copy the text of the example_time_automation.yaml file to a new Automation in Home Assistant.
    - This will update the time every minute.
    - This requires an time entity    
2. To get some more advanced information. Create a second Automation and copy the example_inforamtion_automation.yaml into it.
    - For this you need a input boolean helper entity called "input_boolean.helper_display_info_matrixclock"
    - For this to work you need to change the Automation from the first step to the code of example_advanced_time.yaml
    - This will show information like weather and grocery delivery from PicNic.
        - For the weather information there is a translation. This should be changed to your preferred language 😉
    - This will reoccurs every 5 minutes