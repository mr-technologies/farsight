# `farsight`

Most basic and general sample application is called `farsight` and is located in `samples/01_streaming` directory of IFF SDK package.
It comes with example configuration file (`farsight.json`) demonstrating the following functionality:

* acquisition from XIMEA camera
* writing of raw data to DNG files
* color pre-processing on GPU:
  * black level subtraction
  * histogram calculation
  * white balance
  * demosaicing
  * color correction
  * gamma
  * image format conversion
* automatic control of exposure time and white balance
* H.264 encoding
* RTSP streaming
* HTTP control interface
