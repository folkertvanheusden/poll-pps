poll-pps
--------

This program is for the raspberry pi.
It waits for a PPS pulse on GPIO18 (pin 12).
This is then fed to the NTP daemon via shared memory:

    server 127.127.28.0 minpoll 2 maxpoll 10 prefer


building
--------

It requires the 'wiringpi' package to be installed.
Then:

    make


Written by Folkert van Heusden <mail@vanheusden.com>
License is apache license v2.0
