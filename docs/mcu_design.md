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

#### ADC Peripheral

#### Debayering

#### Color Correction

#### Compression & SPI Peripheral

#### USART Peripheral