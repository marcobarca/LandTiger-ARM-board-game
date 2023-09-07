# Development of a game on LPC1768_HY-LandTiger ARM board

## 1. Init phase and start
The game.c file implements both the game logic and all the functions needed to design the game interface. 
The RIT was used to manage the joystick, while the Timer0 was set up to manage the TouchPanel.
When the project is compiled and the Run button is pressed, the components useful for operation, 
present in the sample.c file, are initialized, the game interface is designed, the calibration procedure is performed and the message 
"Touch to start a new game" appears on the screen, at this point Timer0 is enabled, in order to manage the user's pressure when it expires.
When the Timer0 expires, and the user pressure will be captured by the timer handling, the start() function will be called, 
which initializes the position and direction of the robot, deletes the above message and draws the grid. 
It is important to note that before returning control to the caller, the start() function enables RIT, 
so the game is made available to user commands via the joystick. Note that the timer for how the software has been 
structured will continue cyclically to check if the Restart or Clear buttons on the screen have been pressed.

## 2. Playing
As mentioned above, the RIT manages the joystick, so according to the mode (Explore or Move) 
it will make the robot perform different actions. When the use of the joystick is detected, if the mode 
of use is Explore (mod = 1) the function switch_direction () is simply called, which modifies the position 
of the robot according to the direction indicated in the value "d" and in turn calls the draw_arrow() 
function to draw the robot, and the proximity_sensor() function to find obstacles on the path. 
If the mode of use is instead Move (mod = 2) whatever the direction of the robot, a check is made on 
the latter and modified if different from the previous one, then the run() function is called and it 
performs a preliminary check on the cell towards which you intend to move the robot, to make sure that 
this is not an obstacle or the perimeter of the grid.
For both modes there is a control to prevent intermediate directions of the joystick such as North-East 
for example from having an effect on the robot.

## 3. Victory
When the run() function identifies the cell towards which the robot is moving as an output cell, 
the RIT is disabled in order to inhibit the functionality of the joystick, the grid is cleaned and the you_win()
function is called and it draws a the “YOU WIN” message that is displayed by coloring the cells of the grid green.

## 4. Clear e Restart
Pressing the Clear and Restart buttons on the screen is managed by the Timer0 which, 
thanks to a software mechanism, does not allow them to be pressed in certain circumstances, 
for example when the start message is shown on the screen, it is not possible to press either button; 
when the game is won and the message "YOU WIN" is shown on the screen it will not be possible to press the Clear screen button, 
so the only way to start a new game is via the Restart screen button or the physical Reset button, which restarts the 
card from the initialization phase.

## Finite State Machine
![alt text](/finite_state_machine.png)

## μVision screenshot
![alt text](/screenshot.png)

## LPC1768_HY-LandTiger Manual
https://os.mbed.com/media/uploads/wim/landtiger_v2.0_-_manual__v1.1.pdf

## Youtube 
You can find a brief explanation about the most critical parts of the project on YouTube: 
https://www.youtube.com/watch?v=2nCr6gZmRwQ
