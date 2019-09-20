# GPIO Homework

**Note:** Finish your homework and demonstrate the program in CYT Rm3007B to any of the software seniors before 26/9.

**Construct a program that contains different modes and fulfills the following requirments:**

### Normal Mode: Binary digit operation [60 pts]

The three LEDs represent three bits.</br>
Initially, the value is 0, i.e. all LEDs are off.

- Button 1 will perform increment. i.e. the value will be increased by 1. E.g. 010 -> 011, 011 -> 100 (binary)
- Button 2 will perform binary rotation. i.e. all the bits will be reversed. E.g. 001 -> 100 (binary)
- Button 3 will perform left shift. i.e. all bits moved to the left by 1 bit, the rightmost bit will be filled by 0. E.g. 011 -> 110, 001 -> 010 (binary)
- Holding Button 1 and Button 2 for 2 seconds will reset the value to 0.

### Special Mode: Driving up the wall [40 pts]
- When a button is clicked thrice within 1 second, it would be considered a triple-click.
- If Button 2 is triple-clicked, enter Special Mode. ***[20 pts]***
  - The first two clicks are going to do binary rotation, so the value wouldn't be changed (reverse, then reverse back). However, the board will go into Special Mode when Button 2 is pressed for the third time; and the value should not be altered.
- In Special Mode, the LEDs should perform this sequence `n` times, with `n` being the value in Normal mode (e.g. Value is 101 (binary), then the sequence will repeat 5 times):
  - Keep silent for 500ms, meanwhile display the 3-bit value with the LEDs.
  - Beep with the buzzer for 500ms, turn off all LEDs.
- After the sequence ends, return to Normal Mode automatically.
- Your demo ends here if you choose not to do Bonus Mode.

### Bonus Mode: Stupid lock [20/30 pts]
- In Normal Mode, when Button 1 and Button 3 are held together for 2 seconds, it will go into Bonus Mode.
- The initial state when entering Bonus Mode should be The Hint Sequence.

#### The Password
- The password is a sequence of keystrokes.
- The default password should be Button 1 -> Button 2 -> Button 3.
- You have 2 options for implementing the password.
  - **A:** Fixed length with 3 keystrokes. **[10 pts]**
  - **B:** Variable length up to 8 keystrokes. **[20 pts]**
  
#### State: Changing the Password
- To make your device secure, it's very important to change your password frequently. To change the password, while in The Hint Sequence state, you shall hold all three buttons together for 2 seconds. After that, you should hear a beep sound that last for 100 ~ 500ms to indicate you have entered the changing password mode.
- Depend on your option, implement the following:
  - **Plan A:** Press the three buttons in any order (shall not be repeated, repeated keystrokes should be ignored). After the sequence is completed, enter The Hint Sequence state automatically.
  - **Plan B:** Press the three buttons in any order, the same button can be pressed more than once this time. If you have already entered 8 keystrokes, it should enter The Hint Sequence state automatically. Or if you want to set a password shorter than 8 keystrokes, you should hold all three button for 2 seconds in order to go to The Hint Sequence state.

#### State: The Hint Sequence
- Sometimes, it's very hard for you to remember all the password that you have created since you were young. Therefore, it's a good practice to leave some hints for yourself to open the lock :D
- LEDs should blink to reveal the sequence of the password. E.g. the password is Button 1 -> Button 2 -> Button 3, then the LEDs shall **repeat** the following sequence:
  1. LED 1 turn on
  2. LED 1 turn off
  3. LED 2 turn on
  4. LED 2 turn off
  5. LED 3 turn on
  6. LED 3 turn off
- Each steps should last around 100 ~ 500ms.
- Longer password works the same way.
- You may blink all LEDs once in sync to indicate the end of a cycle, before starting the next loop (optional, for your convenience only when demoing).

#### Back to Normal
- While in The Hint Sequence State, if you entered the password correctly, the device should go back to Normal Mode.
- Your demo ends here if you choose to do the Bonue Mode.
- *Hint: Make it so that for a password of length `n`, ignore all inputs that are more than `n` inputs ago. This way if you pressed a wrong button, you can immedately start over without any additional steps.*


**Remark:**
- The Bonus Mode is an optional task. You should finish the Normal Mode and Special Mode before challenging yourself. **No mark** will be given in Bonus Mode if you haven't completed the above modes.
- There is two task in Bonus Mode, which they shouldn't be coexist. Thus, you can only choose to do either one of the task in a single demonstration.
- Plan A is easier than Plan B, you should choose to do which task base on your ability.
