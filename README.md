# upp_TestVisa
Small U++ project to test Virtual Instrument Software Architecture api. (nothing more)

Test:
-----

 The bin folder contains a statilcally linked exe.
 (hopefully works on your side). Connect the DMM by USB or LXI/TCP. (GPIB is not tested yet)
<BR>
<img src='doc\ss1.png'>
<BR>

Build:
-----

It can be build with ultimate++ environment.
 - Download link: https://www.ultimatepp.org/www$uppweb$download$en-us.html
 - Download Agilent IO Library Suite: www.agilent.com/find/iosuite/
 - Configure the build environment adding the bin, include and lib path at Setup->Build methodes.
 
It was tested on Keysight 34465A at this moment. (The DMM is the only type of instruments which is implemented, probably works with other manufacturers DMMs.)

Develop:
--------

Planned Instruements to add soon:
 - DC Power supply
 - Function generator
 - Oscilloscope (?)
 
Todos:
 - Measurement ranges shall be updated by methode and instrument. (?)
 
