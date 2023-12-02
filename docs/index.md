---
layout: page
title: Project Overview
permalink: /
exclude: true
---

# Project Abstract

NeoObscura is a fully functional camera sensor with on-device image compression built from scratch using an array of 1,200 phototransistors. The MCU will read data from the sensor array, one phototransistor at a time. Once a full image is read, it will be sent to the FPGA over SPI to be compressed. The compressed image will be sent back to the MCU which will send it over UART to a laptop which will interpret and display the image.

<!-- <div style="text-align: left">
  <img src="./assets/img/Logo.png" alt="logo" width="100" />
</div> -->


# Project Motivation
Nearly everyone carries around a smartphone capable of taking high resolution photos and videos, but few people truly understand how a digital camera works. In this project, we aim to learn about the entire process of generating an image, from photon to disk. We want to design and build each part of the camera from scratch: the lens and body, sensor, image processor, and compressor. By creating a simple, modular camera system we hope everyone can get a better understanding of how they work.

# System Block Diagram
![Schematic of system 2](./assets/diagrams/system.png)