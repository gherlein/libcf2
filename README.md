# libcf2
C language library for interfacing to the CrazyFlie 2.0 quadricoptor

The [CrazyFlie 2.0 quadricopter] (https://www.bitcraze.io/crazyflie-2/) is an amazing project that has open sourced just about everything.  While it comes with very complete python libraries I decided I wanted to implement C libraries for my own use.

##Status
This is very much a work in progress.  I think the USB is getting set up right, and I think I am sending some of the right things to the cf2.  But at the moment I don't think I am getting the right answers back.  

THIS CODE IS NOT YET WORKING.

##References
This code draws reference from the [BitCraze Wiki](https://wiki.bitcraze.io/doc:crazyflie:crtp:index#crtp) as well as from their [python code](https://github.com/bitcraze/crazyflie-clients-python)

##Dependencies
This code requires [libusb 1.0](http://www.libusb.org/wiki/libusb-1.0)

##Platforms
This code has only been compiled and tested on OSX.  I plan on making it work on linux as well though, since I want to use it on a Raspberry Pi.


## Credits
Inspired and assisted by reviewing the [libcflie](https://github.com/fairlight1337/libcflie) code by [Jan Winkler](https://github.com/fairlight1337)

## License
This code is licensed GPL3.  I am both giving back and adhering to the licenses of the code that inspired my work.
