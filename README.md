This is my final project about embeded image processing with stm32WB55RG and OV7670 camera module.
Some parst of this repo is taken from : https://github.com/anasvag575/STM32WB55_OV7670_Project/tree/master

I basically benefited from the below Arduino code for camera register settings. I tried this also with Arduino and it works.
https://circuitdigest.com/microcontroller-projects/how-to-use-ov7670-camera-module-with-arduino

For Stm, the register part a litte bit different, look here
https://embeddedprogrammer.blogspot.com/2012/07/hacking-ov7670-camera-module-sccb-cheat.html

I capture grayscale image, which is in YUV format. In this format, one pixel data also comes with 2 bytes and first byte carries brightness info so it is used for grayscale image directly.

You can look the captured and processed images on the related directories.

Feel free to ask you problems, I will glad to improve the repo.


