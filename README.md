# NeoObscura
See the [project website](kavidey.github.io/NeoObscura/) for an overview of the project.

## GitHub Structure

The project is organized into the following folders:
- `hardware`: contains the schematics and PCB layouts for the sensor PCB,  mezzanine breakout PCB, and breadboard schematic.
- `software`: contains the source code for all the software components of the project.
    - `fpga`: SystemVerliog code for QOI compression, and address decoding.
    - `mcu`: C code for the MCU firmware (reading images from the sensor, debayering, interfacing with the FPGA, and sending images over USART).
    - `laptop`: Python code for displaying and saving images from the MCU.
- `hardware`: contains the models for the camera housing, lens mount, and bayer filter designs.
- `docs`: contains the documentation for the project as well as a demo video.
