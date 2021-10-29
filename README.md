# Function keys stuck fix for Framework Laptop

Keys that have another function ability can stick on Linux, as described in this post:  

## Fixing most keys by forcing synthetic key release

Read documentation here: <https://github.com/systemd/systemd/blob/main/hwdb.d/60-keyboard.hwdb>

To fix all the keys except `Brightness Down`, `Brightness Up`, and `Projector` using systemd/udev's synthetic force release ability:  

1. Clone this project
2. `cd` folder and run  
   `sudo cp 70-keyboard-framework-fix.hwdb /etc/udev/hwdb.d` or just do it manually by
   1. Creating `/etc/udev/hwdb.d/70-keyboard-framework-fix`
   2. Paste in this file [70-keyboard-framework-fix.hwdb](70-keyboard-framework-fix.hwdb)
3. Run  
   `sudo systemd-hwdb update`  
   `sudo udevadm trigger --verbose --sysname-match="event*"`  
   (This may need to be run more than once, due to a possible bug, to actually change.)
4. Verify changes with: 
   `udevadm info  /sys/class/input/event2`

   ```console
   E: KEYBOARD_KEY_3b=!F1
   E: KEYBOARD_KEY_3c=!F2
   E: KEYBOARD_KEY_3d=!F3
   E: KEYBOARD_KEY_3e=!F4
   E: KEYBOARD_KEY_3f=!F5
   E: KEYBOARD_KEY_40=!F6
   E: KEYBOARD_KEY_41=!F7
   E: KEYBOARD_KEY_42=!F8
   E: KEYBOARD_KEY_43=!F9
   E: KEYBOARD_KEY_44=!F10
   E: KEYBOARD_KEY_57=!F11
   E: KEYBOARD_KEY_58=!F12
   E: KEYBOARD_KEY_90=!previoussong
   E: KEYBOARD_KEY_99=!nextsong
   E: KEYBOARD_KEY_a0=!mute
   E: KEYBOARD_KEY_a2=!playpause
   E: KEYBOARD_KEY_ae=!volumedown
   E: KEYBOARD_KEY_b0=!volumeup
   E: KEYBOARD_KEY_b7=!sysrq
   E: KEYBOARD_KEY_c7=!home
   E: KEYBOARD_KEY_c8=!up
   E: KEYBOARD_KEY_c9=!pageup
   E: KEYBOARD_KEY_cb=!left
   E: KEYBOARD_KEY_cd=!right
   E: KEYBOARD_KEY_cf=!end
   E: KEYBOARD_KEY_d0=!down
   E: KEYBOARD_KEY_d1=!pagedown
   E: KEYBOARD_KEY_d2=!insert
   E: KEYBOARD_KEY_d3=!delete
   E: KEYBOARD_KEY_ed=!media
   ````

### Speedrun (use at your discretion)

```console
sudo cp 70-keyboard-framework-fix.hwdb /etc/udev/hwdb.d && sudo systemd-hwdb update && sudo udevadm trigger --verbose --sysname-match="event*" && sudo systemd-hwdb update && sudo udevadm trigger --verbose --sysname-match="event*" && udevadm info  /sys/class/input/event2
```

## Brightness Down / Up keys fix

### [Link to custom driver](driver/)

## Keys fixed

| Key                        | Fixes?                                         |
| -------------------------- | ---------------------------------------------- |
| F1                         | Yes                                            |
| F2                         | Yes                                            |
| F3                         | Yes                                            |
| F4                         | Yes                                            |
| F5                         | Yes                                            |
| F6                         | Yes                                            |
| F7                         | Yes                                            |
| F8                         | Yes                                            |
| F9                         | Yes                                            |
| F10                        | Yes                                            |
| F11                        | Yes                                            |
| F12                        | Yes                                            |
| (Fn+F1) Volume Down        | Yes                                            |
| (Fn+F2) Mute               | Yes                                            |
| (Fn+F3) Volume Up          | Yes                                            |
| (Fn+F4) Previous Song      | Yes                                            |
| (Fn+F5) Play Pause         | Yes                                            |
| (Fn+F6) Next Song          | Yes                                            |
| (Fn+F7) Brightness Down    | No, can with [custom driver](driver/README.md) |
| (Fn+F8) Brightness Up      | No, can with [custom driver](driver/README.md) |
| (Fn+F9) Projector          | No, probably can with a anoter custom driver   |
| (Fn+F10) Airplane Mode     | N/A, works fine to begin with                  |
| (Fn+F11) Print Screen      | Yes                                            |
| (Fn+F12) Framework (Media) | Yes                                            |
| Delete                     | Yes                                            |
| (Fn + Delete) Insert       | Yes                                            |
| Left Arrow                 | Yes                                            |
| Right Arrow                | Yes                                            |
| Down Arrow                 | Yes                                            |
| Up Arrow                   | Yes                                            |
| (Fn+Left Arrow) Home       | Yes                                            |
| (Fn+Right Arrow) End       | Yes                                            |
| (Fn+Down Arrow) Page Down  | Yes                                            |
| (Fn+Up Arrow) Page Up      | Yes                                            |
