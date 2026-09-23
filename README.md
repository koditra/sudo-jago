# sudo jago

sudo jago is a custom alarm clock built around the XIAO ESP32-C3.

It has a custom PCB, 3D printed case, ST7789 display, buzzer, and a 3x3 button matrix.

## Features

- Custom PCB
- Custom 3D printed case
- ST7789 TFT display
- 3x3 button matrix
- Built-in buzzer
- Creates its own Wi-Fi network
- Web portal for setting the time and alarm
- Button sequence required to stop the alarm

## Schematic

<img width="958" height="676" alt="Screenshot 2026-09-23 at 4 13 19 PM" src="https://github.com/user-attachments/assets/2084922b-0076-464b-9c8c-00f9d786d57f" />

## PCB

<img width="553" height="685" alt="Screenshot 2026-09-23 at 4 13 41 PM" src="https://github.com/user-attachments/assets/06627718-fc24-4aa9-81bf-d3ca74297383" />


## PCB Render

<img width="765" height="828" alt="Screenshot 2026-09-23 at 4 14 53 PM" src="https://github.com/user-attachments/assets/3030e5e8-d3e8-4128-9f66-219e7734fd7a" />

## 3D Case

<img width="801" height="519" alt="Screenshot 2026-09-23 at 4 11 55 PM" src="https://github.com/user-attachments/assets/5d2232e8-2ec8-4ef5-a89b-2e89564325d0" />

## Full Assembly

<img width="801" height="519" alt="Screenshot 2026-09-23 at 4 11 26 PM" src="https://github.com/user-attachments/assets/af04a189-9d63-42c4-a860-842a664077a9" />

## Hardware

- Seeed Studio XIAO ESP32-C3
- ST7789 TFT
- 9 buttons
- Buzzer
- Custom PCB
- 3D printed enclosure

## Firmware

The ESP32 creates its own Wi-Fi network called `sudo jago`.

Connect to it and use the web portal to set the clock and alarm. The phone can then disconnect and sudo jago runs by itself.

When the alarm goes off, a random sequence appears on the display. The physical buttons must be pressed in the correct order to stop the alarm.

## Built By

Aashvik Goel
