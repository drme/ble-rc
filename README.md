# Introduction

![logo](http://farm6.staticflickr.com/5544/11486171456_9d6423d0d6.jpg)

Radio controlled cars come with some big radio remotes for controlling them. Those remotes are not really convenient to carry especially then the car model is 1:28 or so, remote is 3-4 times bigger than actual car. Also, each car has its remote, so if we have several cars we have to have several remotes.
So let’s build our own remote – use Smartphone for controlling the cars. In addition, we can use 1 Smartphone to control all cars. The car will provide some API for making more interesting controlling – like automated driving.

# Needed steps
* Get the RC car model
* Make RC cars controlling board
* Install new board into the RC car
* Load app into the Smartphone
* Drive

# Quick build instructions
* Checkout solution from the github (url)
* Manufacture PCB (using fritzing file, and oshpark or friztizng service)
* Build Bluetooth control board firmware and flash it.
* Assemble the board.
* Install the board in the car.
* Load app into the iPhone.

#Car analysis

The simple RC car usually contains one brushed motor for rear wheels driving and one bushed motor for its steering. The driving is limited to the full speed backward and full speed forward. The steering is limited as well, the motor can turn wheels to the left or to the right at the maximum angle, and there are no intermediate driving positions besides centre value. Also then the wheels are turned to some side the motor is stalled, as it keeps turning to the desired position but some gears prevent it from going in that direction – thus motor stalls and gets very warm.

![car with 2 motors - drive and steering](http://farm3.staticflickr.com/2861/11606373965_cc0fe1d6e2.jpg)

In other cars, steering is controlled using magnets – each front wheel is connected to the magnet which attracts the centre knob of the steering mechanism then the voltage is applied to the steering magnet, so if we want to make a left turn, we apply voltage to the left magnet, and if we want to make a right turn, we apply voltage to the right magnet.

![Opened and old internals](http://farm8.staticflickr.com/7325/11457735275_1a2f780873.jpg)

Some cars also has built-in lights, what’s strange that those lights are not controllable, but just light up than you go forward or backwards.

And finally the car contains some control board that accepts commands from the remote control pad and drives motors, lights, and steering magnets. This board consists of a receiver module and a h-bridge for driving motors (usually just a couple of transistors or FETs are used to make a h-bridge instead of a h-bridge chip) and some transistors for controlling steering magnets or lights.

So let’s build our own controlling board with additional features:
* Controllable by a smartphone instead of a remote controller.
* Several cars can be controlled by the same smartphone (not at once).
* Adjustable driving speed.
* Individually controllable lights (if available).
* Adjustable steering (if possible depending on steering mechanism setup).

# Designing solution

For making car controllable by Smartphone, we will remove its control board, build a new one with a Bluetooth capability, install it in the car, and use the app on the smartphone for controlling it. The whole solution should look like this:

![design scheme](http://farm6.staticflickr.com/5502/11606712194_d67a52a278.jpg)

* Smartphone with an app – for controlling the car.
* Cars control board, for controlling the car and consisting of:
* Bluetooth module – for phone communication and control. The selected cc2541 module contains Bluetooth module and MCU in one SoC package.
* H-bridge - for driving motors.
* Transistors and resistors – for driving LEDs and steering magnets.
* Voltage regulator and capacitors – for providing stable power to the Bluetooth module.
* PCB – for assembling all components into control board.
* Some firmware for Bluetooth module.
* The actual car

# Designing the board

The board was designed using the frizzing tool. It’s a good idea to start from designing schematics first and then lay outing components on the PCB for board manufacturing. The schematics are presented in the next picture.

![schematics](http://farm3.staticflickr.com/2828/11606919184_f88be787bc.jpg)

The h-bridge, that drives motors, is direly connected to the Bluetooth module. The Bluetooth module, through h-bridge, can control motors rotation direction and if driven by PWM signal can control the rotation speed as well.

The steering magnets are controlled by two BJT NPN transistors, then the current flows through the transistor base, the transistor opens and it connects the magnets one wire to the ground. When this connection is made, the magnet starts working and attracts the steering magnet, thus turning the wheels. The other wire of the magnet has to be connected to the battery positive side.

The lights are controlled in the similar way as the magnets are. The transistor opens then the current flows through its base, and it connects a LED cathode to the ground. Then this connection is made, the current flows from the battery positive side through the resistor and transistor to the ground and it lights up the LED. The Bluetooth module can control LEDs by applying high or low signals on its connected pins.

The voltage regulator provides power to the Bluetooth module. The selected module requires max 3.3V for operation. In some cases, the car is driven by several batteries, whose provide more voltage (like 5V) than Bluetooth module can handle. This voltage regulator regulates voltage down to the 3.3V and allows running Bluetooth module of all kind of batteries.

The designed PCB consists of two layers and contains SMD components, as in some cases it has to be squeezed into some really small cars. The designed PCB can be easily manufactured using its design files in companies such as oshpark.

![pcb top](http://farm4.staticflickr.com/3713/11606570185_578a1ff707.jpg)
![pcb bottom](http://farm3.staticflickr.com/2846/11607326586_7dd208aaf3.jpg)

Then the PCB is available, the components can be soldered onto it, but before soldering, it would be better to flash initial firmware into the Bluetooth module, which later could accept over the air updates.

![assembled board](http://farm3.staticflickr.com/2820/11606974104_4a55f29fbc.jpg)

# Firmware

The control board main part is a cc2541 module by Texas Instruments. This module provides 8051 CPU and Bluetooth radio in one package. The firmware has to be developed using IAR tool that is available as trial version, which is capable of building small firmware images or works for a limited time. Also TI BLE-stack is needed.

The main part of firmware is its Bluetooth Smart GATT table that defines device services and characteristics. So this firmware has one service for controlling a car and this service has several characteristics/attributes:
* 0xacc2 - Steering attribute – the value from 0 to 700, there 350 is centre, 0 is a left turn and 700 is a right turn (and values in the between could be used for precise control).
* 0xacc1 - Throttle attribute – the value from 0 to 700, there 350 is keeps motor off, 0 lets it run backward at full speed and 700 lets it run forward at its full speed (the intermediate values could be used for controlling motor speed, make it run a little bit slower or faster).
* 0xa101 - Lights attribute – depending on written values, the Bluetooth module turns on/off the specified lights. (16-bit number for controlling each individual light. 0x0001 – first light on, 0x0002 -  second light on, 0x0004 third light on, 0x0008 – fourth light on, 0x0010 – fifth light on.)
* Battery voltage attribute – allows monitoring cars battery voltage and to determine if the battery is not running low.

Then the service and its attributes are defined, it’s only left to finish firmware – add code for reacting to attribute changes and adjust motors, steering or lights.

<< Debugger connected>>

Then the firmware is ready it’s time to upload it to the Bluetooth module. The easiest way is to load Over the Air firmware in it, which would allow later updating Bluetooth module firmware directly from a smartphone.

For loading over the air firmware:
* The project has to be built as Simple-Car-OAD-ImageA target
* The BIM project from BLE SDK has to be built as well.
* Connect the Bluetooth module to the CC Debugger.
* Launch Flash Programmer.
* Flash BIM image with Erase, Flash, Verify option.
* Flash Car image with Append, Flash, Verify option.
* If flash was successful, it’s time to assemble the board and proceed to the iOS Application.

![flashing](http://farm4.staticflickr.com/3823/11606971935_23a49751d0.jpg)

![building](http://farm3.staticflickr.com/2860/11607727906_c66104863a.jpg)


# iOS Application

iOS application
Finally, for controlling the car we have to load iOS Application to the smartphone. The application provides the search window for finding all cars and connecting to the selected one. Then the car is connected the applications provides the control window where the steering is controlled by accelerometer (the phone can be rotated as accrual cars wheel) and throttle is adjusted by virtual joystick on the screen. As well, the steering and throttle sensitivity can be adjusted in the settings windows

![cars list](http://farm3.staticflickr.com/2850/11606935884_a4c995c073.jpg)
![controlling](http://farm3.staticflickr.com/2885/11607356396_ba4b7c019d.jpg)

What this application does, it takes values from the accelerometer and the throttle joystick and writes those values to the Bluetooth control chip GATT table. Then those values are written, the control board interprets then and drive the car.

The application is available at the app store, or its source can be checked out from github and compiled with xcode tool.

# Assembling the car

Let’s upgrade some simple car. Here we have the nice car for 1:53 scale. This car can go forward, backwards and make turns to the left and the right. The car is controlled with some remote. If you have several of those tiny cars you have to have a separate remote for each of them.

![Initial candidates](http://farm6.staticflickr.com/5542/11457771644_1deb837f0e.jpg)

Requisites:
* Previously created board.
* Smartphone.
* Smartphone application.
* Some car for hacking.
* Soldering iron and some solder.
* Pliers

![unpacked stuff and mcu](http://farm4.staticflickr.com/3772/11457798944_b8b70aa43d.jpg)

So let’s install our constructed Bluetooth module inside of this tiny car and get rid of remote controller, and as a bonus, we could use just one phone for controlling all cars. The cars’ sharing with the friends could be just by passing a car and installing an app on the smartphone.

![Opened and old internals](http://farm8.staticflickr.com/7325/11457735275_1a2f780873.jpg)

So let’s open this, car. First we have to remove batteries. Inside we have the printed circuit board for receiving commands from remote and controlling the car. The car is driven by small brushed motor; the steering is controlled by two electro-magnets. Then we want to make a turn, the controller applies some current on one of the magnets ant magnet attracts the steering magnet to the desired side and it in turn turns the wheels.

![disconnecting old controll borad](http://farm3.staticflickr.com/2866/11457784774_bae8183fc5.jpg)

So the next step is to remove old control board. We have to disconnect motor – cut be cut or de-soldered, it’s recommended to leave them as long as possible (in this case motor wires are blue and white ones). After that, we have to disconnect battery wires – cut the red and black wires (or de-solder) from the board. The black one goes directly from the battery contacts and the red one goes from the power switch.
Finally, we have to disconnect steering magnets. Those magnets are connected with some tiny wires. Each magnet is connected with 2 wires, depending on how those two wires are connected magnet attracts or pushes steering magnet (reverse the battery for push). In this car, the + sides of both of the magnet is connected to the VCC pad on the control board (it’s recommended to leave those two wires connected together, as we will need than to solder to the new board together as well). The other two wires control the left and right side magnets, those has to be disconnected as well.

![adding connectors](http://farm6.staticflickr.com/5486/11457748105_bfb1e336dc.jpg)

The next step is adding connectors. The connectors can be added to the motor and power wires. In the example selected connectors does not need soldering, they required just some work with pliers. We have to strip some isolation from the wire and add metallic connector and its plastic housing.

![soldering sterring magnets](http://farm6.staticflickr.com/5500/11457932383_4dcd30516b.jpg)

The steering wires are more problematic, they are so tiny that there is no easy way to add connectors, so they can be soldered directly to the replacement board. The two connected wires from magnets have to be soldered to the power pad (or could be soldered directly to the battery positive side wire on the car). The remaining wires from the magnets has to be soldered to the two pads on the board – top pad is for the left magnet wire, the second pad for the top is for the right magnet wire.

![connecting motor and power](http://farm4.staticflickr.com/3831/11457875966_af0d498e24.jpg)

Finally, we have to just plug power connector to the power headers on the board and motor wires to the motor connector, place the board inside the card, and reassemble car back (put the cover and screw it).

![reasembling car back and ready to go.](http://farm8.staticflickr.com/7376/11457889043_73c61e84bc.jpg)

Now it’s time to launch the Smartphone app, turn the car on and start racing.

### Troubleshooting
* Car does not start – check if batteries are installed, check if power wires are connected to the swich, check if power connector to the board is not reversed.
* Car goes backwards instead of forward – reverse motor connector.
* Car turns to the left instead of right – check if magnets wires are soldered to correct pads (just reverse them).
* Steering does not work – check if two power wires from the magnets are together soldered to the VCC pad.


