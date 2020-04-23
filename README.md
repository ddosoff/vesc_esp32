# vesc_extender
Very dirty example of Vesc Uart <-> Wifi bridge.


I use TTGO T-Display as ESP32 board.


Pinout:

T-Display 26 - Vesc Uart RX
T-Display 25 - Vesc Uart TX

I got power from Vesc, but seems it's bad idea. With PowerBank to TTGO connection is better. Do not forget to attach Ground between Vesc and T-Display boards.

Use PlatformIO to compile and flash T-Display board.

Finally I found that protocol have to be refactored and UDP used. Because 1 lost packet cause long retransmission delay.
I will implement simple UDP protocol later.
