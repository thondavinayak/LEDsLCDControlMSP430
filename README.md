GB LED Controller with LCD Feedback (MSP430)
This project demonstrates real-time RGB LED intensity control on the MSP430F5529 LaunchPad using PWM (Pulse Width Modulation), with visual feedback on an LCD display. The LED duty cycles are adjusted using pushbuttons and booster pack buttons. The project also includes basic graphics rendering (rectangle and circle) on the LCD to reflect LED states.

Features
🎛️ RGB LED Control:

Adjust intensity (duty cycle) of Red, Green, and Blue LEDs using buttons.

PWM frequency: 800Hz per channel, implemented using MSP430 timers.

🖥️ LCD Display Integration:

Real-time display of current duty cycle values for each color.

Textual and graphical feedback (colored rectangles/circles) corresponding to LED intensity.

⏱️ Timer-based Sampling:

Timers handle PWM signal generation and update using interrupts.

Software debouncing and gradual control using polling and flags.

🔘 Button Functionality:

S1 (P2.1) ➝ Increase Blue LED.

S2 (P1.1) ➝ Reset all duty cycles to 0.

P4.0 ➝ Increase Red LED.

P3.7 ➝ Increase Green LED.

Code Structure
File	Purpose
main.c	Main loop: handles polling buttons, calling PWM updates, updating display.
digital.c	Initializes GPIOs and handles button interrupts.
time.c	Configures timers for PWM generation and manages duty cycle updates using interrupts.
drawings.c	Provides utility functions for drawing and clearing shapes (rectangle and circle) on the LCD.

How It Works
Button Presses are detected via polling or hardware interrupts, triggering duty cycle changes.

PWM Updates are staged and committed via timer interrupts (Timer0 for green, Timer2 for red/blue).

Display Feedback:

The duty cycle value is shown as text on the screen.

A rectangle or circle is drawn depending on the color and intensity level.

Reset Button (S2) resets all LED intensities to 0 and clears the graphics.

Requirements
MSP430F5529 LaunchPad

BoosterPack with LCD Display

TI Code Composer Studio or any MSP430-compatible toolchain

Example Output
When P4.0 (UP) is pressed:

The Red LED intensity increases.

"RED+" is displayed at (40,40) on screen.

Numeric duty cycle is updated.

A red rectangle is drawn to visualize the ON state.

To Build and Run
Clone this repository.

Import into Code Composer Studio.

Flash the program to the MSP430F5529 board.

Interact using the 4 onboard buttons.
