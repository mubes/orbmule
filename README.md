This is an experimental repository. PLease use the upstream unless you *really* know why you're using this one!

ORBMule Development
===================

This is the repository for the ORBMule, a minimal STM32F427 target board for the Orbuculum project featuring a parallel trace connector so it can serve as a reference board for ORBTrace.  Licences for materials are in individual subdirectories.

![Muleboard](https://github.com/orbcode/orbmule/raw/main/support/images/muleboard.jpg)

There is very little that is interesting about this board, and that's the whole idea...it's only purpose in life is to give you a known reference starting point
when first getting started with ORBTrace. It is nothing more than the CPU, with supporting crystal, a few LEDs and a USB connector.

In the `examples/` directory you will find demos using the board;

* `lcd_demo`: Demo using 320x480 ili9488 based SPI panel (demo will still run without this panel present, contributed by Steve Woodford, [short video](https://github.com/orbcode/orbmule/raw/main/support/images/lcd.mov).

* `simple`: Deliberately minimal demo including example gdbinits for debug and trace for blackmagic, pyocd and openocd debug interfaces.

