# PWM & Timers

Previously in Robotics Software: you learned how to send binary digital signals using GPIO

The Problem: digital signals are incredibly limiting, they are only on or off, what if I want to tell a motor to go to a specific position?

The Solution: 
- **Analog signals**: by varying the voltage we can convey information about a continuously varying quantity, better but still restrictive in some ways and, most importantly, hard for hardware to generate (not our problem? maybe, but lets be nice to them)
- **Serial interfaces**: we use consecutive bits to convey more complex information, this is very common and allows to send alot of information, USB & Ethernet, the backbone of modern computer communication is based on this, but there is another way
- **Pulse Width Modulation**: We look into the time domain and use the *amount of time* that a signal is on or off for, to convey information, rather than just the current state

## PWM Signals and their properties

So, what are the properties of a digital signal, varying in time, that we can actually use to convey information?

- **Frequency**: how often does the signal undergo one *complete cycle* (turn on once and turn off once)
    - **Frequency = 1 / T_period**

- **On-time and Off-time**: within one complete cycle, how long is the signal high or low for?
    - **T_period = T_on + T_off**

- **Duty cycle**: What proportion of each complete cycle is the signal on or off for?
    - actually this is a combination of the above 2, but sometimes you should think about this instead. **Duty Cycle = T_on / T_period**
    - A PWM '*signal*' of a duty cycle N% delivers approximately N% of the power of a similar signal that is always on (100% duty cycle) **Average Voltage = Duty Cycle * Peak Voltage**
    - 

![](https://i.imgur.com/Zp5b5Bl.png)

### Aside: PWM vs Analog

- To generate a digital signal, we simply need 2 switches (remember GPIO tutorial?), this may be physical or most commonly a pair of transistor
- To generate an analog signal, we need a complex and carefully calibrated circuit, as we have to vary the voltage with a more fine-grain
- The above gets even worse when the PWM '*signal*' must also carry a large amount of current/power with it
- To read a digital signal, we just have to compare it to a threshold, this is both easier and more resistant to noise 
- To read an analog signal, we again need a complex and carefully calibrated circuit to recognized the exact voltage. So any amount of noise will corrupt the reading

### Some concrete examples

#### Motors

In fact, you have already used PWM, in the workshop, to control the speed of the motor.

Fundamentally, as I said, the duty cycle can be used to vary the amount of power transmitted, and thus can vary the speed of a motor.

Additionally to adjust the direction of the motor, we must switch which terminal of the motor is positive and which is negative. This is achieved by a circuit known as a H-Bridge.

![](https://i.imgur.com/KAMZUIK.png)

Selecting the PWM frequency requires you to consider a few factors. Without going deep into the mechanical or hardware aspects the factors are:

1. MOSFET transistors (most commonly used for motor drivers) consume energy while switching from on to off or back, but not while constantly on/off. So **higher PWM Frequency reduces efficiency** and heats up the motor driver
2. Current slowly increases within motors when given voltage. If PWM frequency is too high the motor will not have enough time to build up to high enough power. So **higher PWM Frequency reduces power and torque** and the relationship between speed and voltage becomes more nonlinear.
3. The switching of power by PWM in the MOSFETs and Motors creates an *audible* noise at the frequency of the PWM and all multiples of it. So **higher PWM Frequency reduces audible noise** by pushing it out of the audible range
4. Increasing the PWM frequency smoothens the current in the motors and **reduces jitter in the rotation**

Putting all of these together the common range for PWM frequency is **5-10kHz**.

#### RC Servos

![](https://i.imgur.com/rUazTGT.png)

A RC Servo is a little motor with a control circuit inbuilt, that allows you to directly control the position, using the on-time of a PWM signal.

Individual servos may differ slightly on the parameters of the PWM signal however the major convention is as follows:

1. PWM Frequency of 50Hz
2. Midpoint is 1.5ms on-time
3. Leftmost position is 0.9ms and Rightmost position is 2.1ms

## How to Generate PWM Signals?

Using Timers. 

Timers can do almost anything related to timing and counting, PWM is just a small part, so it is important to understand the structure of the Timer peripheral, like the figure below from the Reference Manual

![](https://i.imgur.com/3LkS4Ss.png)

### Oh... um, on second thought lets not use that one, here are some simplified diagrams

![](https://i.imgur.com/2twhDgO.jpg)

![](https://i.imgur.com/oojgQFY.png)

Every tick of the MCU clock increments the Prescaler Counter by 1. When this counter hits the **Prescaler Value** it outputs a pulse and then next tick resets to 0.
- This creates an infinite looping counter and a series of pulses at a ***reduced*** frequency compared to the MCU clock
- Essentially this is a clock divider
- The prescaler_frequency = clock_frequency / (prescaler + 1)
- The output of the prescaler defines the smallest unit of time for the rest of the Timer (T_unit = 1/prescaler_frequency)

Every pulse on the output of the prescaler increments the main counter. In a similar fashion to the Prescaler Counter, it resets on the tick after the **Auto-Reload Value**
- Again, this creates a infinitely looping counter with a 
    - period equal to T_period = T_unit * (autoreload + 1)
    - frequency equal to timer_frequency = prescaler_frequency / (autoreload + 1)
- This counter is the basis for all functions of the Timer

In Output Compare mode, each channel compares its individual threshold to the common Counter value and output a high or low signal based on this comparison

```C
//for X=1,2,3,4 (each channel)
if (counter < compareX) {
    outputX = 1;
} else {
    outputX = 0;
}
```
- This generates 4 PWM Signals (from each channel) that have a common frequency but different on-time, off-time and duty-cycle
- The duty cycle is OutputCompare / (AutoReload+1)

### Task 1 (30pts)

Suppose that we want to control a servo using PWM. We know that the signal must have a frequency of 50Hz, and that the servo can recognize a difference in on-time of 4us.

**1.** What is the correct prescaler frequency? *(5pts)*

**2.** What is the required prescaler value? *5pts)*

**3.** What is the correct auto-reload value? *(5pts)*

Now assume the servo could recognize an infinitesimally small difference in on-time. So we would like to generate a PWM signal with as high as possible resolution to take advantage of this fact

**4.** What is the correct timer frequency? *(5pts)*

**5.** What is the required prescaler value? *(5pts)*

**6.** What is the correct auto-reload value? *(5pts)*

### Okay okay but show me the code

Every timer that you can use has a 'handle' associated with it in code. This is basically a struct containing info about the Timer, timer functions usually take this handle as a parameter so it knows which Timer to configure

```C
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;

#define SERVO_TIM (&htim5)
#define MOTOR_TIM (&htim1)
#define ENCODER_1 (&htim4)
#define ENCODER_2 (&htim3)
#define ENCODER_3 (&htim8)
```

There are 4 steps in software:

1. Initialize the Timer for PWM mode (The hard part, done for you)
`TIM1_PWM_Init(), TIM5_PWM_Init()`
3. Set the Prescaler, Autoreload and (optionally) Compare Values
`void HAL_TIM_ConfigTimer(TIM_HandleTypeDef* htim, uint16_t prescaler, uint16_t autoreload);`
4. Start the Timer
`HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t channel)`
5. while(1) change the Compare Value as your robot needs it
`void HAL_TIM_PWM_SetCompare(TIM_HandleTypeDef* htim, uint32_t channel, uint16_t compare)`

See Inc/tim.h and Src/tim.c for more details

### Task 2 (40pts)

***Always*** verify the signal using an oscilloscope with the help of a tutor first, then try it on a servo motor if available.

**1.** Initialize the Servo port using the library with the correct PWM Frequency and an initial position of the midpoint*[10 pts]*

**2.** Now you will have to create a more complex program to move the Servo over time and in response to button inputs. *[30 pts]*

1. Initialize the servo at the leftmost position *[5 pts]*
2. Change the servo position over time such that it reaches the rightmost position in *exactly 1 second* *[10 pts]*
3. Similarly, move the arm back to the leftmost position in 1 second *[10 pts]*
4. Wait at the leftmost position until BUTTON1 is pressed then repeat steps **2 & 3** *[5 pts]*

### Task 3 (30pts)

Remember that motor control requires a 2nd pin, for direction, that is a normal GPIO pin.

We recommend creating helper functions to deal with a motors pwm and direction pin as one

1. Initialize the motor port and timer with a PWM Frequency of 5kHz and a suitable Timer frequency *[5pts]*
2. Each time BUTTON1 is pressed, increase duty cycle by 10%, and each time BUTTON2 is pressed, decrease duty cycle by 10%, limiting to 0 and 100% *[10pts]*
5. Extend the program to accept a duty cycle of -100% to 100%, such that -10% is equivalent to 10% duty cycle with a reverse direction. *[5pts]*
6. Extend the program to work such that button presses change the final value just like the previous parts, but only changes the duty cycle at a rate of 10% per second. [10pts]


## BONUS

These sections are useful in Robotics, especially the internal competition, but for time, simplicity and to encourage you to go out and learn on your own, it will not be covered in class

### Reading PWM Input Signals

In Input Capture mode, each channel reads the signal input and records the value of the counter on a certain edge of the signal (depending on how you configure it)

```C
//for X=1,2,3,4 (each channel)
if (risingEdge(inputX)) { //or fallingEdge
    compareX = counter;
}
```

- 2 Channels may also be programmed to look at the same input pin, but for different edges
- This allows you to measure the on-time and off-time of a PWM signal

### Counting Pulses and Encoders

The Prescaler counter may be clocked from an external source in various ways, allowing you to count pulses or edges at high frequencies. This is useful when you want to read an encoder which is an easy way to track rotational motion accurately

![](https://i.imgur.com/CLdjZi4.png)

