substitutions:
  devicename: matrixclock # devicename change as you want
  xscrollpadding: "32" # width of the matrix in pix

  
esphome:
  name: $devicename
  platform: ESP32
  board: esp32dev


# Enable Home Assistant API
api:
  encryption:
    key: !secret api_key

ota:
  - platform: esphome
    password: !secret ota_password


# Networksetting change as needed. Static IP settings saves boot time and made it more stable for me. Remove the manual_ip part to get dhcp.
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  manual_ip: 
    gateway: 192.168.1.1
    static_ip: 192.168.1.33
    subnet: 255.255.255.0
    dns1: 192.168.1.1
mdns:
  disabled: false

text:
  - platform: template
    name: helper_displaytext_$devicename
    optimistic: true
    mode: text
    id: extratext
  - platform: template
    name: helper_icon_$devicename
    optimistic: true
    mode: text
    id: icon


logger:
  level: INFO

debug:


font:
  - file: _fonts/matrixclocklightletters.bdf
    size: 30
    id: tinyfont
    glyphs:  |
      ! "#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz°

number:
  - platform: template
    name: "$devicename Scroll speed"
    id: scrollspeed
    icon: mdi:speedometer
    optimistic: true
    min_value: 100
    max_value: 300
    initial_value: 120
    step: 10

switch:
  - platform: template
    name: "$devicename Always scroll"
    id: scroll
    icon: mdi:rotate-right
    optimistic: True
    restore_mode: RESTORE_DEFAULT_ON
  - platform: template
    name: "$devicename Icon scroll"
    id: icon_scroll
    icon: mdi:rotate-right
    optimistic: True
    restore_mode: RESTORE_DEFAULT_ON

time:
  - platform: sntp
    id: rtctime


light:
  - platform: neopixelbus
    variant: WS2812
    pin: GPIO32             #Port for the DIN Pin on the matrix 
    num_leds: 256
    type: GRB
    name: "$devicename Light"
    id: mylight
    restore_mode: ALWAYS_OFF
    internal: true 
  - platform: rgb
    name: "$devicename Scrolling Sign"
    id: matrixcolor
    red: output_red
    green: output_green
    blue: output_blue

output:
  - platform: template
    id: output_red
    type: float
    write_action:
      lambda: ;
  - platform: template
    id: output_green
    type: float
    write_action:
      lambda: ;
  - platform: template
    id: output_blue
    type: float
    write_action:
      lambda: ;

display:
  - platform: addressable_light
    id: led_matrix_display
    addressable_light_id: mylight
    width: 32
    height: 8
    pixel_mapper: |-
      if (x % 2 == 0) {
        return (x * 8) + y;
      }
      return (x * 8) + (7 - y);
    rotation: 0°
    update_interval: 50ms
    lambda: |-
          #include <string>
          static int xpos = 0;
          static int displaywidth = 32;
          static int lastupdate = 0;
          const char * text = id(extratext).state.c_str();
          auto values = id(matrixcolor).current_values;
          float brightness = values.get_brightness();
          id(led_matrix_display).get_light()->set_correction(brightness,brightness,brightness);
          Color color(values.get_red() * 255, values.get_green() * 255, values.get_blue() * 255);
          int x_start, y_start;
          int width, height;
          int position = 0;
          if (id(icon).has_state() && !strlen(id(icon).state.c_str())==0) 
          {
            std::map<std::string, esphome::animation::Animation*> icon_state 
            {
              { "clear-night", id(clear_night) },
              { "cloudy", id(cloudy) },
              { "fog", id(fog) },
              { "hail", id(hail) },
              { "lightning", id(lightning) },
              { "lightning-rainy", id(lightning_rainy) },
              { "partlycloudy", id(partlycloudy) },
              { "pouring", id(pouring) },
              { "rainy", id(rainy) },
              { "snowy", id(snowy) },
              { "snowy-rainy", id(snowy_rainy) },
              { "sunny", id(sunny) },
              { "windy", id(windy) },
              { "windy-variant", id(windy_variant) },
              { "clck", id(clck) },
              { "hourglass", id(hourglass) },
              { "temp", id(temp) },
              { "picnic", id(picnic) },
              { "heart", id(heart) },
              { "calendar", id(calendar) },
              { "twitch", id(twitch) }
              
              /* new icons can be added here if also added at the end of this file. Last icon entry does not have a ","at the end. */
            };
            it.get_text_bounds(0, 0, text, id(tinyfont),TextAlign::TOP_LEFT, &x_start, &y_start, &width, &height);
            if(id(scroll).state == false && (width+8) < displaywidth)
            {
              position = displaywidth - ((displaywidth - (width+8))/2);
            }
            else
            {
              position = xpos;
            }
            int printPos = (-(position % (width + $xscrollpadding + 10))) + displaywidth;
            int iconPos = 0;
            if(id(icon_scroll).state == true)
            {
              iconPos = printPos;
            }
            else
            {
              printPos = (-(position % (width + $xscrollpadding))) + displaywidth;
            }
              it.print(printPos + 10, 1, id(tinyfont), color, TextAlign::TOP_LEFT, text);
              it.image(iconPos, 0, icon_state[id(icon).state.c_str()], COLOR_ON, COLOR_OFF);
          } 
          else
          {   
            it.get_text_bounds(0, 0, text, id(tinyfont),TextAlign::TOP_LEFT, &x_start, &y_start, &width, &height); //default Text
            if(id(scroll).state == false && width < displaywidth){
              position = displaywidth - ((displaywidth - width)/2);
            }
            else{
              position = xpos;
            }
            it.print((-(position % (width + $xscrollpadding))) + displaywidth, 1, id(tinyfont), color, TextAlign::TOP_LEFT, text); 
          }
          if(lastupdate + int(id(scrollspeed).state) <= millis() || millis() < lastupdate){
            lastupdate = millis();
            xpos++;
          }

image:
  - id: image_black
    file: _icons/black.gif

interval:
  - interval: 0.1s
    then: 
      lambda: |-
          static std::vector<esphome::animation::Animation*> gif_animations = {
            id(clear_night), id(cloudy), id(hail), id(lightning), id(lightning_rainy),
            id(partlycloudy), id(pouring), id(rainy), id(snowy), id(snowy_rainy),
            id(hourglass), id(picnic)
          };
          for (auto* animation : gif_animations) {
            animation->next_frame();
          }
# More icons can be added here. Just download new gif or png from here https://developer.lametric.com/icons
# right click on the image you want and click save as.
# don't forget to add the icons also to the code.
animation:
    ### Weather icons
    - file: _icons/13345.gif
      id: clear_night
      type: RGB565
      resize: 8x8
    - file: _icons/2283.gif
      id: cloudy
      type: RGB565
      resize: 8x8
    - file: _icons/2154.png
      id: fog
      type: RGB565
      resize: 8x8
    - file: _icons/2441.gif
      id: hail
      type: RGB565
      resize: 8x8
    - file: _icons/23713.gif
      id: lightning
      type: RGB565
      resize: 8x8
    - file: _icons/49299.gif
      id: lightning_rainy
      type: RGB565
      resize: 8x8
    - file: _icons/2286.gif
      id: partlycloudy
      type: RGB565
      resize: 8x8
    - file: _icons/49300.gif
      id: pouring
      type: RGB565
      resize: 8x8
    - file: _icons/2284.gif
      id: rainy
      type: RGB565
      resize: 8x8
    - file: _icons/2289.gif
      id: snowy
      type: RGB565
      resize: 8x8
    - file: _icons/49301.gif
      id: snowy_rainy
      type: RGB565
      resize: 8x8
    - file: _icons/2282.gif
      id: sunny
      type: RGB565
      resize: 8x8
    - file: _icons/52169.png
      id: windy
      type: RGB565
      resize: 8x8
    - file: _icons/15618.gif
      id: windy_variant
      type: RGB565
      resize: 8x8
    ### Stuff icons
    - file: _icons/clock.png
      id: clck
      type: RGB565
      resize: 8x8
    - file: _icons/hourglass.gif
      id: hourglass
      type: RGB565
      resize: 8x8
    - file: _icons/53328.gif
      id: picnic
      type: RGB565
      resize: 8x8
    - file: _icons/temp.png
      id: temp
      type: RGB565
      resize: 8x8
    - file: _icons/7646.png
      id: heart
      type: RGB565
      resize: 8x8
    - file: _icons/10498.png
      id: calendar
      type: RGB565
      resize: 8x8
    - file: _icons/50812.png
      id: twitch
      type: RGB565
      resize: 8x8