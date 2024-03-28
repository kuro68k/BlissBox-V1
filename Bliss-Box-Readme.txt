This is the Readme file for the Bliss-Box source code. It is important to note the version 1.0 is the only version of this code available. Version 2.0 is in development but not for sale or publication as of yet. This file is the documentation for the source code and GPL licenses agreement only.  Also take notice that the source code had to be modified and fit for the ATMEG 168 chip to be compliant with GPL. It was originally planned to be moved to a 328 chip. However that source code was not complete and now moved off publication and on to version 2.0, a work in progress. Thus it is not available for purchase or publicizing. I’m not sure whether I will release 2.0 or not. As a result and to be fully GPL compliant I had to reassemble version 1.0. A lot of the code has been reorganized, modified, and in some cases stripped way down (including formatting and comments in some cases). Most of the code found in this package is written in c, others in asm. This code is available AS-IS with no support or warranty and matches the version used for Bliss-Box version 1.0.

WHAT IS INCLUDED IN THIS PACKAGE
=========================
Readme.txt : The file you are currently reading.
License.txt “ Free Open Source license for this package (GPL).
Folder Update: The code for updating, derived from Object development's bootloader.
Folder source/usbdrv: A copy of the V-usb firmware
Folder schematic: The board design
Root Folder:  Contains the individual source files that handle the reading of various controllers. 


Introduction
=======================
This is the first ever all in one, trouble free, and ready to go usb adapter. There is no technical knowledge require to use this. You just plug in the controller; plug in the usb adapter and play.  Version 1.0 was intended to support controllers only. This excludes things like guns, paddles, dance pads, multi taps, and robots.
What supported in this version:
N64, (s)NES,GameCube,Virtual Boy,  Most versions of Playstation, Sega 3/6 and MS, Saturn, dream Cast, Atari, 3d0, coleco, neo-geo,tg16, pcfx, and possibly more (untested).
This code was designed to work on a Windows system, may work with others. 

Hardware
See attached schematic in the schematic folder

LICENSE
=======
About USB id's:  
The ID's that are programmed in usbconfig.h for both the updater and Bliss-Box are free Id's, Your welcome to use them as you feel fit. It is strongly encouraged to replace them with your own Id’s. If you’re only using them for private use you should not immediately run in to any troubles.  In order to use both at once you must change one of the device id’s as both won’t work correctly if they are same. 
See USBID-License.txt for more info. 


Bliss-Box and related code is distributed under the terms of the GNU General Public License (GPL) version 2 (see License.txt for details) and the GNU General Public License (GPL) version 3. 

If you intend to use any of this code you are encouraged to inform the author. You must adhere to the GPL licenses and include the following in your code and on your website or other publication.  
Author:   Sean Green 
Contact:   S_K_U_N_X@yahoo.com
WebSite: http://spawnlinux.dyndns.org/Bliss-Box/

----------------------------------------------------------------------------
Bliss-Box version 1.0
(c) 2010 by Sean Green.
http://spawnlinux.dyndns.org/Bliss-Box/
