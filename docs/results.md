---
layout: page
title: Results
permalink: /results/
---

Our camera sensor meets all the specifications we set out for ourselves at the beginning of this project:

1. [x] MCU reads image from sensor
2. [x] MCU and FPGA communicate over SPI
3. [x] FPGA compresses images using the QOI algorithm
4. [x] MCU sends compressed images to laptop over UART
5. [x] Laptop saves and displays compressed images

We created a working 30x40 pixel sensor that outputs 8 bit, full color images at around 1.2 FPS. The limiting factor in our FPS is the serial baudrate of 921,600. With a maximum ADC sampling speed of 5.33 Msps and 1200 samples per frame, our theoretical maximum is 4442 FPS. In practice (taking into account switching delays, CPU computation time, etc.) we achieve a sampling rate of 52 ksps resulting in 43.5 FPS (if we don't account for USART transmission time).

Accounting for USART transmission time (assuming an average compressed image size of 2,500 bytes) we 1.5 FPS.

<!-- 
One frame takes 23 ms = 0.023 s
There are 1200 samples per frame -> 0.0000191667 s per sample -> 52,173.822306396 samples per sec
The limit on the FPS imposed by the ADC is 43.4782608696
 -->

Debayering was not originally in our project proposal because we were not sure if it was possible to manufacture a bayer filter in time and it was not necessary for the electrical camera functionality. After successfully making one using an inkjet printer and printer transparencies we implemented a

Using the FPGA as a hardware accelerator allows us to successfully encode an image efficiently. Each pixel takes a maximum of 52 clock cycles to load, encode, and write to ram, and for 1200 pixels, that is 62,000 clock cycles. Writing the end footer to RAM then takes 40 more clock cycles, giving a total of 62,040 clock cycles. Since the encoder runs at 24MHz, encoding one image takes a maximum of less than 0.0026 seconds. 
<!-- TODO: Add quantitative info about compression (runtime, efficiency, etc.)-->

Compressing the images has a noticeable impact on the maximum FPS we achieve (unfortunately decreasing it). A 30x40 RGB 8 bit image takes up 3,600 bytes but requires sending 7,200 bytes over serial (using encoding each byte as a 2 digit hex number). The theoretical maximum size of a 30x40 8 bit RGB*A* QOI encoded image is 6,100 bytes (12,200 bytes over serial). This would have a worse FPS than sending the raw image however in practice we see an average image size of 2,500 bytes. The QOI compressed image takes less *does* take less time to send, the compression itself and communicating with the FPGA both take a significant amount of time, resulting in 2 FPS.


### Future Work
There are a few improvements we want to make to this project. The first, most obvious one, is to design and manufacture a camera body and associated lens system so that it can be used as an actual camera instead of just a sensor.

Additional features to increase its functionality as a camera including adding an onboard screen for displaying images, and the ability to save files to an SD card.

To address the FPS limit imposed by USART, we also want to use the USB peripheral on the MCU to make the camera act as UVC device work with any computer without the need for python code to receive and display images.
