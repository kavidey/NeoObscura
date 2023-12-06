---
layout: page
title: MCU Design
permalink: /mcu_design/
---

The MCU has 4 roles in this project:

1. Reading frames from the image sensor using its internal ADC
2. Processing the frames (Debayering & Color Correction)
3. Sending frames to the FPGA for compression
4. Sending compressed frames to the computer for display via USART

These roles can be seen in the block diagram below:

#### MCU Block Diagram
<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/mcu.png" alt="MCU Block Diagram" width="400"/>
</p>

There are two main processes on the MCU: the main loop and the ADC interrupt handler. On boot up, the main loop initialized all the necessary peripherals (ADC, USART, SPI, and the FPGA). It also sets up two buffers for storing frame data in. Finally, it start the first ADC conversion (which will trigger the ADC interrupt handler, shown as a dotted arrow in the diagram).

**Main Loop**

In the infinite loop the MCU waits for the current frame to be finished. As soon as the frame is finished it swaps the frame buffers and tells the ADC to start capturing the next frame. While that is happening it debayers the image to go from greyscale to full color and sends it to the FPGA for compression (this is blocking rather than being done with interrupts). When the FPGA responds with the compressed image the MCU sends it over USART to be displayed on the laptop (see [Python Script](#python-script)).


**ADC Interrupt Handler**

Each time the ADC interrupt handler is triggered it saves the current pixel into the active frame buffer. If it is not on the last pixel, it configures the analog muxes to select the next pixel and starts a new conversion. If it is on the last pixel, it sets a flag to tell the main loop that the current frame is done.

<!-- TODO: move color correction code from the ADC interrupt to the main loop -->

#### ADC Peripheral

#### Debayering

#### Color Correction

#### Compression & SPI Peripheral

#### USART Peripheral

See [results]({{ site.baseurl }}/results/) for more information the maximum frame rate we were able to achieve.

##### Python Script

To display images from the camera we wrote a python script that reads the serial output from the MCU and displays the images in a window. The script can be found in our github repository [here](https://github.com/kavidey/NeoObscura/blob/e155/software/laptop/display_image.py). The script uses the [PySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) library to read the serial output, [OpenCV](https://opencv.org/) to display the images, and [qoi](https://pypi.org/project/qoi/0.0.3/) to decode the images.

It supports reading QOI compressed images, or raw color or greyscale images (all of which can be selected with command line arguments). The script also supports displaying the images in a window and saving them to a file.