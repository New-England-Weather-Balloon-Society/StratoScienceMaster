# StratoScienceMaster
# The masta code


As part of our class called "StratoScience Lab," 2 of our students are testing Ozone levels in the atmosphere, and the other is measuring wind levels and the power generated from it.

To conserve power, as this will be flying on our next high altitude balloon, titled, "HABGab 2023", we need to conserve power and weight coming from batteries.
This Arduino sketch combines code that reads from UV sensors (you can get Ozone from this), a BMP388 Temperature and Pressure (to get altitude readings) sensor, and a wind experiment that uses analog pins to measure voltage and watts from a motor on the payload cord.

The BMP388 measures altitude to also trigger two of our cutdown systems, developed by one of our students, to release 2 projects, a glider and an unpowered helicopter from the payload.
