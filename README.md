# Raspberry Pi Pico Project

## Video demo
<video width="100%" controls>
  <source src="./PIOStarter Demo (compressed).mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

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
   Implement a CPU interrupt and then shorten the PIO pause to match the interrupt.  
   Consider addressing latency issues associated with pausing only at the top of the cycle.

6. **Object-Oriented Programming (OOP) Firmware Architecture**  
   Use OOP style firmware as a proof of concept for proper structure.
   
## Highlights/Things to Note

1. **PIO is fast and efficient**  
   While the E-stop for the LED has some latency due to the clock cycling, the speed at which it resumed shows how fast it can be.
   Furthermore, you can abstract passive tasks like LED management to PIO to save CPU space for important things like the motor controller.
   Lastly, you can load in values for the PIO so it still has the benefits of being a regular function.
   
3. **Customizable Firmware Architecture**  
   The organization into a central app and several modules allows for the app to pick and choose which features to use.

4. **Config file for customizing hardware interface**  
   I just checked and I realize that the existing firmware does something pretty similar with pinout, but the vibes still stand.

5. **Slightly Worse **  
   Blink speed now changes based on the motor controller speed.  
   Test that there’s no need for feeding the state machine when empty (check pull no block).
   
## Future Ideas to Implement

- **I2C Data Collection**  
  Collect I2C data either on the side or through PIO. I believe the sensing board has 4 I2C connections, which is too many.

- **DMA Controller**  
  If we implement any communication protocols over PIO, we will probably need to load it directly into memory due to bandwidth constraints.

- **Better Comments/Documentation**  
  I vibed some of these comments and debugging, hence the weird syntax and arrow symbols, so need to write real comments to actually make this easy to understand.

## Resources
- **What is Programmable I/O on Raspberry Pi Pico?**  
  [Read the article](https://magazine.raspberrypi.com/articles/what-is-programmable-i-o-on-raspberry-pi-pico)

- **Official Pico SDK Documentation**  
  [Read the docs](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html)

- **How to Use PIO**  
  [Blog post by Greg Chadwick](https://gregchadwick.co.uk/blog/playing-with-the-pico-pt4/)

- **For Beginner-Friendly Help**  
  [Talk to me like I'm stupid](https://forums.raspberrypi.com/viewtopic.php?t=356489)
  
## Work on the Project

If you are interested in working on this more, please visit **"Jason's Firmware Dump"** on Notion:  
[Jason's Firmware Dump](https://www.notion.so/gt-mrg/Jason-s-Firmware-Dump-2d14d4efb4c780429ba9cd4f02c7532a)
