# Raspberry Pi Pico Project

## Video demo
<video src="https://github.gatech.edu/user-attachments/assets/e94030c5-7b0a-4ca9-8309-5265b8febaf2" controls>
  Your browser does not support the video tag.
</video>
Video is too large, go watch it on the Notion page below


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

4. **Easily change the constants**  
   Due to modular design, one can easily change constants specific to certain modules, such as the clock divider of the PIO.
   
## Future Ideas to Implement

- **I2C Data Collection through PIO**  
  Added regular I2C in main CPU context, but I believe the sensing board has 4 I2C connections, which is too many for a Pico without PIO.

- **DMA Controller**  
  If we implement any communication protocols over PIO, we will probably need to load it directly into memory due to bandwidth constraints.

- **Better Comments/Documentation**  
  I vibed some of these comments and debugging, hence the weird syntax and arrow symbols, so need to write real comments to actually make this easy to understand.

- **Make this work for our firmware**  
  This is just a general base to build off of when developing our framework/architecture, so we need to actually make this work for our boards. (duh!)
  See Notion page below for list of tasks.
  
- **Add the Protobuf and ROS Handlers**  
  See previous bullet point

- **Debugging capabilities**  
  Look into picoprobe

- **Remove Build Flags in Library**  
  This removes the whole shove bunch of build flags in a big main file, but still requires build flags because the compiler tries to compile all files in lib.
  Theoretically Dead Code Elimination means you shouldn't need to worry about that, but I put flags just to quiet the errors for now, and also moved the flags from config to platform.ini to make them more global.
  Could consider Interface-based design or the Null Object Pattern is apparently a thing where you make a null object that acts as a stand-in to brick when called, eliminating the need for all build flags outside of the classes themselves.

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

If you are interested in working on this more, please visit **"Jason's Firmware Dump"** on Notion:  
[Jason's Firmware Dump](https://www.notion.so/gt-mrg/Jason-s-Firmware-Dump-2d14d4efb4c780429ba9cd4f02c7532a)
