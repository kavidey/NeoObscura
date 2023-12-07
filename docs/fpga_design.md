---
layout: page
title: FPGA Design
permalink: /fpga_design/
---

The FPGA is used as a hardware accelerator to perform QOI image compression. The QOI (Quite OK Image) format was created with the intent of creating fast, lossless compression with a relatively simple implementation. The format’s entire specification fits on just one page, and does not contain any hardware unfriendly math like JPEG’s discrete cosine transform. 

Images are encoded row by row, left to right, top to bottom. Each pixel is encoded as one of the following:

1. A run of the previous pixel
2. An index into an array of previously seen pixels
3. A difference to the previous pixel value in r,g,b
4. The full r,g,b or r,g,b,a values 

For a more in depth specification of QOI, see [QOI Specification](https://qoiformat.org/qoi-specification.pdf).

The overall layout of the FPGA design can be seen below:

### FPGA Block Diagram
<embed src="{{ site.baseurl }}/assets/diagrams/fpga_block_diagram.pdf" type="application/pdf" width="100%" height="500em"/>
<br>

#### SPI Module
The first task of the FPGA is to shift in the 1200 RGBA pixels into RAM over SPI from the MCU. Once it is done writing the pixels to RAM, the SPI module will set the pixelsReady signal high, which will tell the encoder module to begin encoding. Once the SPI module reads that the encoder is done encoding (when doneEncoding is high), it will shift out an information code of 0x80 to the MCU to indicate that the encoded image is ready, before shifting out the encoded bytes. To create this SPI functionality, we wrote a peripheral interface from scratch to align with a SPI polarity of 0 and phase of 1.

#### Encoder Module
Once the SPI module signals that the pixels have been written to RAM, the encoder module begins the QOI compression process. Following the QOI specifications, the encoder writes anywhere from 0-5 bytes to RAM for each pixel according to previously discussed encoding techniques. This process takes place in an FSM, shown below, that, upon completion, writes the doneEncoding signal high, ensuring that the SPI module knows when to begin shifting out the encoded image.

### Encoder FSM Diagram
<embed src="{{ site.baseurl }}/assets/diagrams/fpga_encoder_fsm.pdf" type="application/pdf" width="100%" height="500em"/>
<br>

#### RAM Module
Since both the SPI and Encoder modules need to read and write to/from RAM, the RAM module helps to handle the necessity of two controllers. This module essentially muxes each of the SPI Ram I/Os with each of the Encoder Ram I/Os to allow for the use of just one hardware defined RAM block.

#### Top Module 
The Top module simply instantiates 1 instance of each of the above modules, as well as the internal high frequency oscillator to be used as the clock. One important thing to note is that the clock signal that is fed into the encoder module runs at half the speed of the clock signal fed into the RAM module to remove the potential for metastability on the RAM I/Os. Also, the SPI clock (sck) must run at a speed of at least half of the internal high frequency oscillator, which runs at 48 MHz, for the same reason. 

#### 2:4 Decoder
Alongside the QOI Compression logic, the FPGA also implements a 2:4 decoder to decode addresses for the analog muxes. For more details on why this is implemented, refer to [Interfacing with Image Sensor](https://kavidey.github.io/NeoObscura/mcu_design/#interfacing-with-image-sensor).
