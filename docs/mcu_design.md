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

##### Main Loop

In the infinite loop the MCU waits for the current frame to be finished. As soon as the frame is finished it swaps the frame buffers and tells the ADC to start capturing the next frame. While that is happening it debayers the image to go from greyscale to full color and sends it to the FPGA for compression (this is blocking rather than being done with interrupts). When the FPGA responds with the compressed image the MCU sends it over USART to be displayed on the laptop (see [Python Script](#python-script)).

##### ADC Interrupt Handler

Each time the ADC interrupt handler is triggered it saves the current pixel into the active frame buffer. If it is not on the last pixel, it configures the analog muxes to select the next pixel and starts a new conversion. If it is on the last pixel, it sets a flag to tell the main loop that the current frame is done.

<!-- TODO: move color correction code from the ADC interrupt to the main loop -->

#### ADC Peripheral

The ADC is setup to read the voltage output of a single pixel on one of its 16 external channels. We are using the ADC at its maximum resolution of 12 bits and maximum sampling rate of 5.33 Msps (Note: the ADC sampling rate is not the limiting factor in our FPS, see [results]({{ site.baseurl }}/results/) for more details). Nonetheless, the ADC is driven using the system clock at the maximum speed of 80 MHz to reduce sampling time as much as possible. We are using the ADC in left aligned mode with offset disabled so that the magnitude of the output does not depend on the ADC resolution.

There are several ways to start an ADC conversion (single conversion mode, continuous conversion mode, and hardware or software triggers). We chose to use software triggers to start all of our conversions because we want the highest possible sampling speed but also need to change the state of the analog muxes between each sample. As shown in the [MCU Block Diagram](#mcu-block-diagram), conversions are started in software from the main loop (at the beginning of a new frame) or in the ADC Handler Interrupt after the muxes have been setup for the next pixel.

#### Interfacing with Image Sensor

There are 3 components to interfacing with the MCU, row selection, column selection, and output.

Row selection uses a 32:1 analog mux that is built into the sensor PCB. There are 30 rows so 5 address bits are required to select the necessary row.

Originally, we planned to use 3x 16 channel external ADCs and read from all columns at once, however we scaled down the design to be more achieveable in the timeline of this project (that would have required a 3rd custom PCB that we didn't have time to create). Because of that change, we opted to use three 16:1 analog muxes ([CD74HC4067](https://www.sparkfun.com/products/9056)) on our breadboard to control the 40 columns. Using three 16:1 muxes instead of one 64:1 mux requires slightly more complicated addressing logic than for the row selection. The MCU outputs a 6 bit signal indicating which column it wants to select. The *lower* 4 bits are wired to all three 16:1 muxes while the *upper* 2 bits are wired to a 2:4 bit decoder on the FPGA that controls the enable pin on each of the three 16:1 muxes, only activating the correct one based on the column the MCU wants to select.

All of the address selection is done using the GPIO pins on the MCU.

[ADC Peripheral](#adc-peripheral) and [ADC Interrupt Handler](#adc-interrupt-handler) have more details on exact process of setting up the ADC and reading individual pixels. Below is a trace of the voltage input to the ADC while one whole frame was being captured.

<p align="center">
    <img src="{{ site.baseurl }}/assets/img/sensor_trace.png" alt="Sensor Oscilloscope Trace" width="600"  style="image-rendering:pixelated;"/>
</p>
<h5 align="center">
Sensor Oscilloscope Trace
</h5>

Note that the ~120 Hz component of the signal is due to the flicker of the fluorescent lights in the digital lab.

#### Image Processing

The output of the image processing pipeline is a 8 bit 30x40 QOI encoded image. We chose to use the ADC in 12 bit mode even though the final image is 8 bit to prevent [color banding](https://en.wikipedia.org/wiki/Colour_banding) and other image artifacts caused by insufficient color resolution.

<div class="mermaid" align="center">
graph LR
    A(Color Correction) --> B(Debayering)
    B --> C(QOI Compression)
</div>
<h5 align="center">
Image Processing Pipeline
</h5>

##### Color Correction

The brightness and contrast of the image straight from the camera sensor depend on a lot of factors including: the ambient light and the resistor used in the phototransistor circuit.

To correct the brightness and contrast, we use alpha-beta adjustment as described by Richard Szeliski in Computer Vision: Algorithms and Applications, 2nd ed.

The pixel $$f(i,j)$$ in the input image is adjusted using the following equation:

$$g(i,j) = \alpha \cdot f(i,j) + \beta$$

We set $$\alpha = 1.1, \beta = -0.1$$ to create an image had a reasonable amount of contrast and black level close to 0.

<!-- TODO: Add before and after images -->

##### Debayering
A physical Bayer filter covers the camera. The pixel locations determine the "color" of each value.

<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/debayering/filter.png" alt="Bayer filter pattern" width="400"/>
</p>

The filter is applied to the sensor, and we obtain a single channel image

<p align="center">
    <img src="{{ site.baseurl }}/assets/img/single_channel.png" alt="Simulated raw sensor output of course logo" width="400" style="image-rendering: pixelated;"/>
</p>


Debayering is the process of converting this single channel image (visualized below in grayscale) into an image with full color per each pixel.

Each color channel of each color pixel is calculated as a linear interpolation of adjacent values of the respective color.

Each color pixel can therefore be obtained from the values of the 3x3 grid of single-channel values centered on its location.

<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/debayering/kernel.png" alt="Channel calculation" width="400"/>
</p>


This 3x3 grid covers out-of-bounds values when calculating pixels on the border of the image. 


<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/debayering/overlay.png" alt="Filter on border pixel" width="400"/>
</p>

Naively, one would extend the border pixels as necessary to fill the filter input. 
This violates the Bayer patern, which may instead be preserved by copying/mirroring the row/column one removed from the edge.
Pixels on two edges (i.e. corner pixels) are accounted for by sequentially mirroring horizontally/vertically, where the ordering making no difference. Pre-handling the (literal) edge cases enables the 3x3 kernel logic to remain simple.

<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/debayering/edge_handling.png" alt="Edge handling" width="400"/>
</p>

This edge-handling diagram extends rotationally to all other edges and corners.


It would be theoretically ideal that in the case of a green corner, the diagonally inferred pixel would be sampled from the corner itself rather than the green pixel one removed from both edges. 

This ends up not mattering as the 3x3 kernel samples channels directly on the same color tile, so the green channel of the green corner Bayer tile corner is sampled directly anyways, irrespective of what green pixels are mirrored.

We proceed to shift the 3x3 kernel to be centered at all the coordinates of the image until the full color image is calculated.

<p align="center">
    <img src="{{ site.baseurl }}/assets/diagrams/debayering/progression.png" alt="Filter traverse pattern" width="400"/>
</p>

The final result looks as follows.

<p align="center">
    <img src="{{ site.baseurl }}/assets/img/debayered.png" alt="Course Logo, Debayered" width="400" style="image-rendering: pixelated;"/>
</p>





#### Compression & SPI Peripheral

The MCU and FPGA communicate using over SPI using a custom sequence of commands as shown below:

<div align="center">
    <div class="mermaid" style="width: 500px;">
        sequenceDiagram
            participant M as MCU
            participant F as FPGA
            Note over M,F: Reset FPGA
            M->>F: Send 1200 pixels (4 bytes each for RGBA)
            M->>F: Tell the FPGA we're done sending pixels (Send 0x08 4x)
            par FPGA compresses image
                F-->>F: Compress image
            and MCU waits
                M->>M: Wait for compression to finish
                Note right of M: Continue reading<br/>until response is not<br/>0x80 or 0x081
            end
            F-->>M: Send compressed image
            Note right of M: Continue reading<br/>until last 8 bytes<br/>match footer
    </div>
    <br/>
    <h5>
        Compression Sequence Diagram
    </h5>
</div>


<!-- M->>F: Reset FPGA
Note right of F: 1. Raise FPGA Reset<br/>2. Send 0xAF (2x)<br/>3.   Lower FPGA Reset -->
<!--
The first task of the FPGA is to shift in the 1200 RGBA pixels into RAM over SPI from the MCU.
Once it is done writing the pixels to RAM, the SPI module will set the pixelsReady signal high, which will tell the encoder module to begin encoding. Once the SPI module reads that the encoder is done encoding (when doneEncoding is high), it will shift out an information code of 0x80 to the MCU to indicate that the encoded image is ready, before shifting out the encoded bytes -->

The SPI peripheral is setup with a 5 MHz clock with polarity of 0 and phase of 1. There is only one device on the SPI Bus (the FPGA) so the chip select line is replaced with a reset signal for initalizing the compression algorithm each time before use.

The full reset sequence involves:

1. Raising the reset line
2. Writing 0xAF twice
3. Lowering the reset line

See [FPGA Design]({{ site.baseurl }}/fpga_design/) for a detailed explanation of how compression is implemented.

#### USART Peripheral

The primary goal for this project is to build a functioning camera not to drive a display, so we chose USART as simple protocol for sending images to a laptop.

After the image is captured, color corrected, debayered, and compressed, it is converted to hex and sent over USART to the laptop. We chose specifically to use USART_2, which transmits through the built in Micro-USB port on the Nucleo board for simplicity. We are transitting with a baudrate of 921,600 which was the highest we could go without running into signal integrity issues and dropping a significant number of frames.

See [results]({{ site.baseurl }}/results/) for more information the maximum frame rate we were able to achieve.

##### Python Script

To display images from the camera we wrote a python script that reads the serial output from the MCU and displays the images in a window. The script can be found in our github repository [here](https://github.com/kavidey/NeoObscura/blob/e155/software/laptop/display_image.py). The script uses the [PySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) library to read the serial output, [OpenCV](https://opencv.org/) to display the images, and [qoi](https://pypi.org/project/qoi/0.0.3/) to decode the images.

It supports reading QOI compressed images, or raw color or greyscale images (all of which can be selected with command line arguments). The script also supports displaying the images in a window and saving them to a file.
