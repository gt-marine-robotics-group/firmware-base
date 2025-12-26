# Marine Robotics Group Firmware Foundation  
As RoboSub undertakes its electrical re-design and MRG's firmware move towards developing interoperable hardware that can be easily implemented in systems for its different teams/domains, the opportunity to refactor our firmware has arisen. Given the Raspberry Pi Pico's capabilities, we had several goals in mind: 
- exploring the usage of Programmable IO (PIO) on the Pico (custom state machines that handle IO in the background)
- configuring multiple boards from a single codebase to allow for standardized yet flexible firmware  

Our solution is Product Line Architecture, a framework for creating lines of related products that share commonalities with planned differences. Through Modular Design and Object-Oriented Programming, we can encapsulate our features into objects selectively included in customized board configurations. Thus, our development becomes compartmentalized, enabling multiple projects to progress in parallel.  
This Raspberry Pi Pico Project is a proof-of-concept using SparkFun parts, where we can reconfigure a Pi Pico to run different builds by switching environments in PlatformIO. This project includes firmware that demonstrates primitive implementation of our planned functionalities, including I2C, PIO, Interrupts, and PWM Outputs (missing Protobuf). That said, this framework is a foundation to build upon, and there is a long list of tasks to develop the various starter modules into full-fledged features.

## Highlights/Things to Note

1. **PIO is fast, efficient, and partitioned**  
   While the E-stop for the LED has some latency due to clock cycling, the speed at which it resumed shows how fast it can be.  
   Furthermore, you can abstract passive tasks like LED management to PIO to save CPU space for important things like the motor controller.  
   The PIO still runs even when the CPU stops, unless you short the interrupt pins together. This allows you to save states.  
   Lastly, you can load in values for the PIO so it still has the benefits of being a regular function.  
   
2. **Customizable Firmware Architecture**  
   The organization into a central app and several modules allows for the app to pick and choose which features to use.  
   You can still keep central features like an E-Stop while deciding what sensors and peripherals you want.  
   This sample project has an example of every type of interface except SPI: PIO, I2C, PWM Output, and Interrupts.  

3. **Config file for customizing hardware interface**  
   I just checked and I realize that the existing firmware does something pretty similar with pinout.h, but the vibes still stand.

4. **Modular Design**  
   Due to modular design, one can easily change constants specific to certain modules, such as the clock divider of the PIO, and those changes don't affect other PIO interfaces. Furthermore, each specific module is highly self-sufficient, and the board file only needs to call and run it.

## Project Overview
[Current State of Project Library Directory](https://github.com/gt-marine-robotics-group/firmware-base/blob/main/PIOStarter/lib/README.md)  

- **Presto Board**  
  Motor Controller Board featuring
  - 8 PWM outputs for Motor Control
  - 2 Independent PWM outputs
  - E-Stop
  - Status Light (LED Interface)
  - RC interface (not yet addressed)

- **Sensor Board**
  Sensor Interface featuring
  - 3 I2C Sensors (BME280, INA219, Blue Robotics Pressure Sensor)
  - Digital Leak Sensors
  - NeoPixel RGB LED (not yet addressed)

## In-progress/Future Ideas to Implement
The current list of project goals. Feel free to add or handle these, but if you take something off the list, please add it to the Project Iterations section below, so we can easily keep track for documentation and progress purposes. These are grouped into three categories and ordered by difficulty: Advanced Features, Implementation, and Framework & Architecture Design.

### Advanced Features  
This is essentially stretch goals or developing new features. Our basic framework covers simple firmware features, but given the cool possibilities enabled by the Pico, this is where we puruse stretch goals with PIO, DMA, and Protobuf.

- **SPI Data collection through PIO - INTERMEDIATE**  
  Implementing SPI Data collection through PIO. Should be the easiest the three, but not easy at all.
  [PIO SPI](https://github.com/raspberrypi/pico-examples/tree/master/pio/spi)
  
- **DMA Controller - ADVANCED**  
  If we implement any communication protocols over PIO, we will probably need to load it directly into memory due to bandwidth constraints.
  There's an example using UART to do it, so we should proably start there and then port it over to SPI/I2C:  
  [PIO UART DMA](https://github.com/raspberrypi/pico-examples/tree/master/pio/uart_dma) 
  [PIO UART RX](https://github.com/raspberrypi/pico-examples/tree/master/pio/uart_rx) 
  [PIO UART TX](https://github.com/raspberrypi/pico-examples/tree/master/pio/uart_tx)
  
- **I2C Data Collection through PIO - ADVANCED**  
  Added regular I2C devices in main CPU context, but I believe the sensing board has 4 I2C connections (which could probably be just 2 or even 1), but 4 is too many for a Pico without PIO.
  There is a pretty reliable-looking example on the Pico Examples GitHub: 
  [PIO I2C](https://github.com/raspberrypi/pico-examples/tree/master/pio/i2c)

- **I2C DMA through PIO - ADVANCED**  
  This the previous two combined to make your life an absolute nightmare. Should be fun. :)

- **Embedded Control/ML - ADVANCED**  
  Assuming we get through the PIO gauntlet above, we have now freed up a signficant amount of compute/processing power on our main CPU, as PIO is handling everything. This can be used for real-time embedded machine learning, control, and data processing (including error handling) applications. Will be a lot of fun if we can get that far.

### Implementation  
This codebase is only a proof-of-concept, we need to actually do the hard part now and use this framework to implement our specific features. A helpful resource is the skeleton code that Mitchell originally laid out when we were rushing to meet deadlines.

- **SPI Sensor - BEGINNER**  
  The advanced goal is to make a fancier SPI communication interface using PIO. This one is just set up an SPI sensor and is pretty easy, but I don't actually have an SPI sensor with me during break. Should take like 10 minutes to set up since there are plenty of examples, and I also have reference code from ECE4180 if needed. If I'm being honest, I'm not sure if we have any SPI sensors, but it's good to keep in our back pocket.
  
- **Motor Controller - BEGINNER**  
  I have this mostly laid out, is just some GPIO mapping and tuning the constants. Could become INTERMEDIATE if you want to do some fun stuff with structs and Protobuf.
  
- **Make this work for our firmware - INTERMEDIATE**  
  This is just a general base to build off of when developing our framework/architecture, so we need to actually make this work for our boards. (duh!)
  See Notion page below for list of tasks.
  
- **Add the Protobuf and ROS Handlers - INTERMEDIATE/ADVANCED**  
  Make a designated package for Protobuf packing and interfacing with ROS.

- **Merge in ROS Handlers from old/skeleton code - INTERMEDIATE/ADVANCED**  
  See previous bullet point, but also if I'm completely honest, I only somewhat understand the RC Framework. Like I can see what it's doing, but I don't want to dive into the weeds of it. Also clean up the syntax cuz it's a little messy.

 ### Framework & Architecture Design
 This framework is a rough draft, there are plenty of opportunities to improve it. Most improvements are either conceptual design choices or improving the robustness of our framework.
 
- **Better Comments/Documentation - BEGINNER**  
  Mostly cleaned up by the Doxygen documentation, but can always be better, especially regarding the logical implementation.

- **Draw the line between modularity and centralization - BEGINNER**  
  Decide if we want to use config.h as a central controller for all hardware constants (like clocking rates for peripherals or colors for LED interface), or if we want to use config.h as a pinout.h (and maybe set a few flags) and encapsulate the hardware constants in their specific modules, with the specific tweaking being done through flags in config.h

- **RTOS - BEGINNER/INTERMEDIATE**  
  Everything is currently super looped, we can add optionality for RTOS if we want more concurrent operation. Should be fairly straightforward (BEGINNER) if you're just implementing it, might be closer to INTERMEDIATE if you start doing system design stuff and timeslicing with your modules.

- **Memory and Packet Design - INTERMEDIATE**  
  Protobuf is packed pretty well, and at the moment we don't have much memory usage, but this is just something to look into for more efficient communication.

- **Debugging capabilities - INTERMEDIATE/ADVANCED**  
  Look into picoprobe. Also seems pretty straightforward since there are plenty of examples, but I only have one Pi Pico with me. Could also consider just using a simple debug UART to make life easier.

- **Further compiler related considerations - ADVANCED**  
    Theoretically Dead Code Elimination means you shouldn't need to worry about #ifdef in the library, but I put flags just to quiet the errors for now, and also moved the flags from config to platform.ini to make them more global.  
  Could consider Interface-based design or the Null Object Pattern is apparently a thing where you make a null object that acts as a stand-in to brick when called, eliminating the need for all build flags outside of the classes themselves.
  The key to all of this seems to be in how the LDF scans the libraries to find dependencies. On that note, althought ChatGPT says it's impossible, I really want to nest my folders in the lib folder but it doesn't work.
  If we really want more control, we can switch to CMake, but that seems like way too much work.


## Project Iterations  

1. **Blink**  
   Simple LED blink functionality.
   
2. **Blink Pauses on GPIO Input**  
   Blink pauses when there is a GPIO input change.

3. **Add Motor Controller**  
   Integrating a motor controller to control motor speeds.

4. **Blink Tied to Motor Controller Speed**  
   Blink speed now changes based on the motor controller speed.  
   Test that there’s no need for feeding the state machine when empty (check pull no block).

5. **E-Stop**  
   Implement a CPU interrupt and shorten the PIO delay to match the interrupt (latter is deprecated).  
   Consider addressing latency issues associated with pausing only at the top of the cycle.

6. **Object-Oriented Programming (OOP) Firmware Architecture**  
   Use OOP style firmware as a proof of concept for proper structure.

7. **Added I2C Sensors**  
   For other board configuration, added a Temp/Humidity sensor to demonstrate using an I2C Sensor.  
   Also added a second IMU (9DOF Stick) onto the same bus to ensure I2C is working fine.  
   Can easily switch between 2 board configurations (defined by build flags).

8. **LED Mux**  
   Given that most boards have an LED interface, added a simple led mux package to instantiate and selectively turn on GPIO pins.
   Might modify the masking logic to give more control since it's currently just a case-switch statement

9. **Better Comments/Documentation**  
   Added Doxygen Comments to literally every single file. Still might need some improved clarity, especially regarding logical implementation.

10. **Improve Board Specific Functionality and Remove Build Flags in App Flow**  
    Made board specific files instead of AppCore lol. That's what I outlined in my original Discord message, and eliminates the need for a bunch of build flags in the logical flow, which was the annoying part. This tackles the whole shove bunch of build flags in a big main file issue, but still requires build flags at the top because the compiler tries to compile all files in lib.  

## Video demo
<video src="https://github.gatech.edu/user-attachments/assets/e94030c5-7b0a-4ca9-8309-5265b8febaf2" controls>
  Your browser does not support the video tag.
</video>
Video is too large, go watch it on the Notion page below

## Resources
- **What is Programmable I/O on Raspberry Pi Pico?**  
  [Read the article](https://magazine.raspberrypi.com/articles/what-is-programmable-i-o-on-raspberry-pi-pico)

- **Official Pico SDK Documentation**  
  [Read the docs](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html)

- **How to Use PIO**  
  [Blog post by Greg Chadwick](https://gregchadwick.co.uk/blog/playing-with-the-pico-pt4/)

- **For Beginner-Friendly Help**  
  [Talk to me like I'm stupid about pull noblock](https://forums.raspberrypi.com/viewtopic.php?t=356489)
  
## Work on the Project
- **RoboSub Skeleton Code**  
  We'll be implementing the functionalities defined here: 
[Mitchell's RoboSub Skeleton Code](https://github.com/gt-marine-robotics-group/mrg-helm/tree/robosub-skeletoncode)

- **Old RoboSub Firmware**  
  That skeleton code was designed to replace this firmware that was previously on RoboSub. You will likely be able to port over functions from it: 
  [old RoboSub firmware](https://github.com/gt-marine-robotics-group/nova-motor/blob/robosub/firmware/firmware.ino)

If you are interested in working on this more, please visit **"Jason's Firmware Dump"** on Notion:  
[Jason's Firmware Dump](https://www.notion.so/gt-mrg/Jason-s-Firmware-Dump-2d14d4efb4c780429ba9cd4f02c7532a)
