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
The ADC is setup to read the voltage output of a single pixel on one of its 16 external channels. We are using the ADC at its maximum resolution of 12 bits and maximum sampling rate of 5.33 Msps (Note: the ADC sampling rate is not the limiting factor in our FPS, see [results]({{ site.baseurl }}/results/) for more details). Nonetheless, the ADC is driven using the system clock at the maximum speed of 80 MHz to reduce sampling time as much as possible. We are using the ADC in left aligned mode with offset disabled so that the magnitude of the output does not depend on the ADC resolution.

There are several ways to start an ADC conversion (single conversion mode, continuous conversion mode, and hardware or software triggers). We chose to use software triggers to start all of our conversions because we want the highest possible sampling speed but also need to change the state of the analog muxes between each sample. As shown in the [MCU Block Diagram](#mcu-block-diagram), conversions are started in software from the main loop (at the beginning of a new frame) or in the ADC Handler Interrupt after the muxes have been setup for the next pixel.

#### Image Processing
##### Color Correction
The brightness and contrast of the image straight from the camera sensor depend on a lot of factors including: the ambient light and the resistor used in the phototransistor circuit.

To correct the brightness and contrast, we use alpha-beta adjustment as described by Richard Szeliski in Computer Vision: Algorithms and Applications, 2nd ed.

The pixel in the input image $$f(i,j)$$ is adjusted using the following equation:

$$g(i,j) = \alpha \cdot f(i,j) + \beta$$

We set $$\alpha = 0.2, \beta = -0.2$$ to create an image had a reasonable amount of contrast and black level close to 0.

##### Debayering

#### Compression & SPI Peripheral

#### USART Peripheral
The primary goal for this project is to build a functioning camera not to drive a display, so we chose USART as simple protocol for sending images to a laptop. 

After the image is captured, color corrected, debayered, and compressed, it is converted to hex and sent over USART to the laptop. We chose specifically to use USART_2, which transmits through the built in Micro-USB port on the Nucleo board for simplicity. We are transitting at 115200 bits/s which was the highest we could go without running into signal integrity issues and dropping a significant number of frames.

See [results]({{ site.baseurl }}/results/) for more information the maximum frame rate we were able to achieve.

##### Python Script

To display images from the camera we wrote a python script that reads the serial output from the MCU and displays the images in a window. The script can be found in our github repository [here](https://github.com/kavidey/NeoObscura/blob/e155/software/laptop/display_image.py). The script uses the [PySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) library to read the serial output, [OpenCV](https://opencv.org/) to display the images, and [qoi](https://pypi.org/project/qoi/0.0.3/) to decode the images.

It supports reading QOI compressed images, or raw color or greyscale images (all of which can be selected with command line arguments). The script also supports displaying the images in a window and saving them to a file.