# R200-UHF-Reader

These are some quick notes for the R200 UHF reader from [Invelion](https://www.aliexpress.us/item/2255800095419099.html?gatewayAdapt=glo2usa4itemAdapt), piggybacking on the documentation here [https://github.com/playfultechnology/arduino-rfid-R200](https://github.com/playfultechnology/arduino-rfid-R200)

This [YouTube video](https://www.youtube.com/watch?v=f9FDgGtX9tk) of the hardware setup leads to [r200_sdk_en.7z](https://drive.google.com/file/d/1F_RoBq9P4xX8C7TYR9cc1TLt00wjUUn3/view)

[BasicTest.ino](BasicTest.ino) contains a simple test that can be done over UART with an Arduino (Uno). Power has to be 5V from the Arduino to the 5V pin on the R200 board, TX/RX pins need to be 0 and 1.

