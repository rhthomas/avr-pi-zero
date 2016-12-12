# avr-pi-0

## Required packages
```
sudo apt-get update; sudo apt-get upgrade -y
sudo apt-get dist-upgrade 
sudo apt-get install bison gcc flex gcc-avr avr-libc
```

## avrdude
```
wget http://download.savannah.gnu.org/releases/avrdude/avrdude-6.2.tar.gz
tar xfv avrdude-6.2.tar.gz
cd avrdude-6.2/
./configure -enable-linuxgpio
make
sudo make install
sudo nano /usr/local/etc/avrdude.conf 
```

## Changes to `/usr/local/etc/avrdude.conf`
```
programmer
  id    = "linuxgpio";
  desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
  type  = "linuxgpio";
  reset = 26;
  sck   = 6;
  mosi  = 19;
  miso  = 13;
;
```

## Check all is working
```
sudo avrdude -c linuxgpio -p attiny85 -v
```

## Pin connections
```
              1 -> PB5 (~RESET)
 8  7  6  5   2 -> PB3
+----------+  3 -> PB4
|   tiny85 |  4 -> GND
|o         |  5 -> PB0 (MOSI)
+----------+  6 -> PB1 (MISO)
 1  2  3  4   7 -> PB2 (SCK)
              8 -> VCC
Raspberry Pi
1  -> VCC
6  -> GND
37 -> GPIO26 (RESET)
35 -> GPIO19 (MOSI)
33 -> GPIO13 (MISO)
31 -> GPIO06 (SCK)
```
