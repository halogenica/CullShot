CullShot
========

Ludum Dare 29 - "Beneath the Surface"


About the game:
---------------
Shoot a CULL SHOT to reveal enemy cubes.
Capture the cubes to increase the time limit.
Stay alive as long as possible. 

XBOX Controller:
Rotate with left stick
Charge shot by holding A
Shoot by releasing A
Capture by pressing X when shot is over cube

PS3 Controller: 
Rotate with left stick 
Charge shot by holding X 
Shoot by releasing X 
Capture by pressing "Square" when shot is over cube 

Keyboard:
Rotate with arrow keys
Charge shot by holding space
Shoot by releasing space
Capture by pressing S when shot is over cube

Extra Controls: 
F to fullscreen 
M to mute music 


A game by Michael Romero
Made for Ludum Dare 29
http://halogenica.net

Links:
------
http://www.ludumdare.com/compo/ludum-dare-29/?action=preview&uid=12262

Gameplay Video 		- http://youtu.be/5WIzH4hpY2k
Timelapse Video 	- http://youtu.be/yG_ARssfVW0
Windows Download 	- https://dl.dropboxusercontent.com/u/1525295/CullShot_LD48.zip
MacOSX Download 	- https://dl.dropboxusercontent.com/u/1525295/CullShot_LD48_Mac.zip


System requirements:
--------------------
Windows 7+ or Mac OSX 10.7+
XBOX (windows) or PS3 (mac) controller recommended



To build from source:
---------------------
Built using VS2012 on Win, Xcode5 on Mac

All project paths are relative, root can be whatever you want. My root (~) is C:/Code/..

Directory trees:
~/games/CULL_SHOT/..                           (directory of this readme, requires all of the below to be built first)
~/libraries/bullet-2.82-r2704/..               (download from code.google.com/p/bullet/)
~/libraries/openal-soft-1.15.1/..              (download from kcat.strangesoft.net/openal.html, build as static lib)
~/libraries/cinder_0.8.6_dev/boost/..          (download from sourceforge.net/projects/boost/files/boost/1.53.0/)
~/libraries/cinder_0.8.6_dev/..                (download from github.com/cinder/Cinder/tree/dev, requies boost to build)
~/libraries/cinder_0.8.6_dev/blocks/Gamepad/.. (download from github.com/halogenica/Cinder-Gamepad)
