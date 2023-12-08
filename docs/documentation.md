---
layout: page
title: Documentation
permalink: /doc/
---

# New Hardware
The main new piece of hardware we're using is a custom designed image sensor (pictured below)

![Camera Sensor]({{ site.baseurl }}/assets/img/sensor.png)

The sensor PCB contains a 30x40 array of 1200 phototransistors along with a 32:1 analog mux (used for selecting rows) and two mezzanine connectors (50 pins each) for communication. The sensor is connected to a breadboard with a 48:1 analog mux (used for selecting columns). The analog muxes (controlled by the MCU) are used to select a single phototransistor to read from by applying voltage to the corresponding column and reading the output voltage from the corresponding row. The phototransistor is setup in a common collector configuration with a XX k resistor between the emitter and ground.
<!-- TODO: enter correct resistor value -->


<p align="center">
    <img src="{{ site.baseurl }}/assets/schematics/common_collector_phototransistor.svg" alt="Common Collector Phototransistor Schematic" width="300">
    <br>
</p>
Schematic Source: electronics-notes.com[^1]

The phototransistors and muxes are all run on 5V. Note that the sensor PCB theoretically supports any 0805 phototransistor, but we are using the [PT0805CS](https://www.alibaba.com/product-detail/0805-SMD-Phototransistor-PT0805CS-Photosensor-Photosensitive_1600907962335.html) due to its low cost and availability. See schematics for the sensor PCB [here](#sensor-pcb).


The new MCU peripheral we are using is the ADC which reads analog signals (in this case from the phototransistors in the sensor) and converts them to a digital signal.

# Schematics
<!-- Include images of the schematics for your system. They should follow best practices for schematic drawings with all parts and pins clearly labeled. You may draw your schematics either with a software tool or neatly by hand. -->
#### Breadboard
<embed src="{{ site.baseurl }}/assets/schematics/breadboard.pdf" type="application/pdf" width="100%" height="500em"/>
<br>
#### Sensor PCB
Here is the schematic of the sensor PCB. It contains the array of 1200 phototransistors, 32:1 analog mux, and mezzanine connectors. See the [bill of materials](#bill-of-materials) for more information on the parts used and links to the source files for the PCB.
<embed src="{{ site.baseurl }}/assets/schematics/sensor.pdf" type="application/pdf" width="100%" height="500em"/>
<br>

# Source Code Overview
<!-- This section should include information to describe the organization of the code base and highlight how the code connects. -->

The source code for the project is located in the Github repository [here](https://github.com/kavidey/NeoObscura/tree/e155).

# Bill of Materials
<!-- The bill of materials should include all the parts used in your project along with the prices and links.  -->

| Item | Part Number | Quantity | Unit Price | Link |
| ---- | ----------- | ----- | ---- | ---- |
| Sensor PCB | Custom | 1 | $8.32 |  [source files](https://github.com/kavidey/NeoObscura/tree/e155/hardware/Sensor)* |
| Mezzanine Breakout PCB | Custom | 2 | $0.69 |  [source files](https://github.com/kavidey/NeoObscura/tree/e155/hardware/MezzanineBreakout)* |
| SMD Phototransistors | PT0805CS | 1200 | $0.08 |  [link](https://www.alibaba.com/product-detail/0805-SMD-Phototransistor-PT0805CS-Photosensor-Photosensitive_1600907962335.html) |
| 16:1 Analog Mux & Breakout | CD74HC4067 | 3 | $5.95 |  [link](https://www.sparkfun.com/products/9056) |
| Female Mezzanine Connector | 0513380574 | 2 | $2.30 |  [link](https://www.digikey.com/en/products/detail/molex/0513380574/2404972) |
| Male Mezzanine Connector | 5017450501 | 2 | $2.11 |  [link](https://www.digikey.com/en/products/detail/molex/5017450501/2818869) |
| 32:1 Analog Mux | ADG732BSUZ | 1 | $13.11 |  [link](https://www.digikey.com/en/products/detail/molex/5017450501/2818869) |
| 5k Resistor | N/A | 1 | N/A |  stockroom |

**Total cost: $145.48**

<!-- 8.32+2\cdot0.69+0.08\cdot1200+3\cdot5.95+2\cdot2.30+2\cdot2.11+13.11 -->
*\*Note: The custom PCBs were manufactured by [JLCPCB](https://jlcpcb.com/).*

# References

Phototransister Circuts Applications. [https://www.electronics-notes.com/articles/electronic_components/transistor/phototransistor-circuits-applications.php](https://www.electronics-notes.com/articles/electronic_components/transistor/phototransistor-circuits-applications.php) Accessed 7 Dec. 2023.

Qoi Image Compression. [https://qoiformat.org/](https://qoiformat.org/) Accessed 7 Dec. 2023.

Technical Data Sheet 0805 SMD Phototransistor: PT0805CS. [http://szsgir.com/data/PT0805CS.pdf](http://szsgir.com/data/PT0805CS.pdf) Accessed 7 Dec. 2023.

<!-- TODO: Make this an actual reference format -->
# Acknowledgements
Prof. Brake for all of his help, support and teaching as we attempted this crazy project. Sean Hodgins for inspiring us to do this in the first place.

# Inspiration
The inspiration for this project came from a simlar project found [here](https://www.youtube.com/watch?v=PaXweP73NT4).

We aim to expland on this project by making it higher resolution, adding a bayer filter for color imagery, and adding on-device image compression.

[^1]: [https://www.electronics-notes.com/articles/electronic_components/transistor/phototransistor-circuits-applications.php](https://www.electronics-notes.com/articles/electronic_components/transistor/phototransistor-circuits-applications.php)
