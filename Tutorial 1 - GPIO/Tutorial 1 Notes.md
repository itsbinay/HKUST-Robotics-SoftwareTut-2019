# Tutorial 1: GPIO

Authors: Joseph Lam, Binay Gurung, Anshuman Medhi

Contact: mhlamaf@ust.hk, bgurung@ust.hk, amedhi@dontemailme.com

## Basic

### Fundamental Program Structure
```C
int main(void) 
{
    // Initialize Everything Here, this is like setup()	
    HAL_Init();	
    SystemClock_Config();	
    MX_GPIO_Init();     //Initialize most GPIOs on board
    MX_DMA_Init();      //Initialize DMA


    //Run code in an infinite loop: this is like loop()
    while (1) {
        static uint32_t last_ticks=0;	//This variable keeps its stored value through every iteration
        //Everything inside this if-statements gets called every 100ms
	if((HAL_GetTick()-last_ticks) >= 100){
	    //Write your code here....
	    last_ticks=HAL_GetTick();
	}
    }
}
```

The HAL_GetTick() function is the main thing to use for timing control in software. It counts up by one every millisecond.

**BTW**: Try not to use `int`, `long`, `long long`, `float`, `double` for embedded system programming. Instead, use the followings:

| Full name | Meaning                  |
| :-------: | :----------------------: |
| uint8_t   | unsigned 8 bits integer  |
| uint16_t  | unsigned 16 bits integer |
| uint32_t  | unsigned 32 bits integer |
| uint64_t  | unsigned 64 bits integer |
| int8_t    | signed 8 bits integer    |
| int16_t   | signed 16 bits integer   |
| int32_t   | signed 32 bits integer   |
| int64_t   | signed 64 bits integer   |

Both full name and short form work.<br>
**Note:** We seldom use `float` and `double` as floating point calculation take more time than integer calculation.

### What is GPIO?
A **general-purpose input/output (GPIO)** is an uncommitted digital signal pin on an integrated circuit or electronic circuit board whose behavior—including whether it acts as an input or output—is controllable by the user at run time.

<!--
 ██████  ██████  ██  ██████       ██████  ██████  ███    ██ ███████ ██  ██████
██       ██   ██ ██ ██    ██     ██      ██    ██ ████   ██ ██      ██ ██
██   ███ ██████  ██ ██    ██     ██      ██    ██ ██ ██  ██ █████   ██ ██   ███
██    ██ ██      ██ ██    ██     ██      ██    ██ ██  ██ ██ ██      ██ ██    ██
 ██████  ██      ██  ██████       ██████  ██████  ██   ████ ██      ██  ██████
-->

<br>

### GPIO Configuration

Every pin on the MCU can be used as GPIO, they are divided into a blocks of 16 pins

Each block is named by letters: `GPIOA, GPIOB, GPIOC...`

Each pin within the block is numbered from 0: `GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2`
- **NOT** `0, 1, 2, 3`

So every pin is referred to by a pair of values: the port and the pin: `GPIOE, GPIO_PIN_3` or when humans are talking we say `PE3`

We will often use `#DEFINE` to give more meaningful names to the ports and pins, for ease of use:

```C
#define BUZZER_GPIO_Port      GPIOE
#define BUZZER_Pin      GPIO_PIN_3
#define LED1_GPIO_Port        GPIOE
#define LED1_Pin        GPIO_PIN_4
#define LED2_GPIO_Port        GPIOE
#define LED2_PIN        GPIO_PIN_5
#define LED3_GPIO_Port        GPIOE
#define LED3_PIN        GPIO_PIN_6
....
```

#### Initialization of GPIO

Any hardware that you want to use must be first initialized, this is basically setting it up so it can work they way you want it to

The `HAL_GPIO_InitPin()` function initializes a GPIO pin with the following parameters:
- `port`: indicates which GPIO port to be initialized **( indicated in main.h )**
- `gpio`: indicates which GPIO pin to be initialized **( indicated in main.h )**
- `mode`: indicates which GPIO mode to be applied for this GPIO pin
- `pull`: indicates which pull mode to be applied for this GPIO pin
```C
void HAL_GPIO_InitPin(GPIO_TypeDef* port, uint16_t pin, uint16_t mode, uint16_t pull);
```

##### *Definition of `GPIO Mode` and `GPIO Pull`*
```C
#define  GPIO_MODE_INPUT                        0x00000000u   /*!< Input Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    0x00000001u   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    0x00000011u   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        0x00000002u   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        0x00000012u   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_AF_INPUT                     GPIO_MODE_INPUT          /*!< Alternate Function Input Mode         */
#define  GPIO_MODE_ANALOG                       0x00000003u   /*!< Analog Mode  */
#define  GPIO_MODE_IT_RISING                    0x10110000u   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   0x10210000u   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            0x10310000u   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */

#define  GPIO_NOPULL        0x00000000u   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        0x00000001u   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      0x00000002u   /*!< Pull-down activation                */
```
  
##### Explanations

- `GPIO_MODE_INPUT` - The GPIO Pin can be used to read the logical value of the pin, low corresponding to 0, high corresponding to 1
    - The pull parameter is relevant here
    - This parameters controls the hardware to attach a pull-up or pull-down resistor
    - The point is to deal with the secret 3rd state of binary digital signals: floating pins. What happens when a pin is connected to nothing at all?
    - Noise will cause you to read a mostly random value
    - The pull-up or pull-down gives a "*weak*" connection from the pin to either a high or low voltage, gives a defined value to a floating pin while being weak enough to be easily overriden by any external signal.

![when the GPIO is connected to a pull down resistor](https://i.imgur.com/sd7FdFC.jpg)

![when the GPIO is connected to a pull up resistor](https://i.imgur.com/iS3Od90.jpg)

- `GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD` - The GPIO Pin can be used to output a digital signal using a pair of switches (see figure below)
    - Push-pull(PP) uses the 2 switches to connect the pin to either high voltage or low voltage, it pushes or pulls the voltage to the level assigned
    - Open-drain(OD) is similar to the above but does not use the upper switch, thus it outputs a low voltage or completely disconnects the pin
    - `GPIO_MODE_AF_PP & OD` are the same but are for when the pin output is to be controlled by another bit of hardware, don't worry about it for now

Inside the MCU lives a pair of switches(transistors):<br>
![](https://i.imgur.com/UerwY9k.png)

- `GPIO_MODE_ANALOG` - The GPIO Pin can be used in conjunction with the builtin ADC to read an analog voltage on the pin, will be covered later

- `GPIO_MODE_IT_RISING, GPIO_MODE_IT_FALLING, GPIO_MODE_IT_RISING_FALLING` - Interrupts will not be covered

##### Example:
```C
/* GPIO Pin Init*/

// configure as output
HAL_GPIO_InitPin(LED1_GPIO_Port, LED1_Pin, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);

// configure as input
HAL_GPIO_InitPin(BUTTON1_GPIO_Port,BUTTON1_Pin, GPIO_MODE_INPUT,GPIO_PULLUP);
HAL_GPIO_InitPin(BUTTON2_GPIO_Port,BUTTON2_Pin, GPIO_MODE_INPUT,GPIO_PULLUP);
HAL_GPIO_InitPin(BUTTON3_GPIO_Port,BUTTON3_Pin, GPIO_MODE_INPUT,GPIO_PULLUP);
```

<br>

#### Reading GPIO Input
The `HAL_GPIO_ReadPin()` function reads the GPIO input with the following parameter:

- `GPIOx`: indicates which GPIO port to be read
- `GPIO_Pin`: indicates which GPIO pin to be read
```C
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```
The GPIO pin read functions returns one of the following states of that respective pin:
```C
typedef enum
{
  GPIO_PIN_RESET = 0u, //The Pin is reading a low signal
  GPIO_PIN_SET         //The Pin is reading a high signal
} GPIO_PinState;
```


##### Example:
```C
// getting the GPIO1 input
GPIO_PinState input = HAL_GPIO_ReadPin(BUTTON1_GPIO_PORT,BUTTON1_PIN);
```

<br>

#### Writing GPIO Output
The `HAL_GPIO_WritePin()` function writes GPIO pin with the following parameters:
- `GPIOx`: indicates which GPIO port to be written
- `GPIO_Pin`: indicates which GPIO pin to be written
- `GPIO_PinState`: indicates what value to write

```C
typedef enum
{
  GPIO_PIN_RESET = 0u, //The Pin should output a low signal
  GPIO_PIN_SET         //The Pin should output a high signal
} GPIO_PinState;

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
```

Alternatively, the following function **toggles** the GPIO pin:
- `GPIOx`: indicates which GPIO port to be toggled
- `GPIO_Pin`: indicates which GPIO pin to be toggled

```C
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```
This function will turn the state of a GPIO PORT/PIN from one state to another:
```
  GPIO_PIN_RESET    ->   GPIO_PIN_SET
  GPIO_PIN_SET      ->   GPIO_PIN_RESET
```

##### Example:
```C
//Set the LED1 to high (turn on)
HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)
//Set the LED2 to low (turn off)
HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET)l

// If LED2 is at GPIO_PIN_SET state, it will be set to GPIO_PIN_RESET state
// If LED2 is at GPIO_PIN_RESET state, it will be set to GPIO_PIN_SET state
HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
```

***Library Reference:***
- `/Src/gpio.c`
- `/Inc/gpio.h`
- `/Inc/main.h`

<!--
██      ███████ ██████       ██████  ██████  ███    ██ ████████ ██████   ██████  ██
██      ██      ██   ██     ██      ██    ██ ████   ██    ██    ██   ██ ██    ██ ██
██      █████   ██   ██     ██      ██    ██ ██ ██  ██    ██    ██████  ██    ██ ██
██      ██      ██   ██     ██      ██    ██ ██  ██ ██    ██    ██   ██ ██    ██ ██
███████ ███████ ██████       ██████  ██████  ██   ████    ██    ██   ██  ██████  ███████
-->

<br>

### Simplest uses for GPIO

On the board that we have given you there are 3 LEDs, 1 Buzzer and 3 Buttons

![](https://i.imgur.com/vbMo4Pa.png)
![](https://i.imgur.com/PGr0gtV.png)
![](https://i.imgur.com/xSSOQvJ.png)

***Thinking Time***: what should the GPIO Mode and Pull be for each of the above and why?

When you want to use a GPIO Pin there are some main steps you should remember:
1. Find which port and pin on the MCU exactly are you trying to use. Most pin definitions can be found in `main.h`
2. Initialize the GPIO Pin once at the beginning of the program. Remember which mode and pull you need for the pin to do its job
3. Use the GPIO Functions to read or write the digital signals as you want

#### LED Example and Classwork

```C
// The following line of code initialises LED1.
HAL_GPIO_InitPin(LED1_GPIO_Port,LED1_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);

// turn on LED1
HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);

// turn off LED1
HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);

// Toggle LED1
HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
```
***Classwork 1** Distributed during tutorial*


#### Buzzer Example and Classwork

```C
// The following line of code initialises BUZZER.
HAL_GPIO_InitPin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);

// turn on BUZZER
HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);

// turn off BUZZER
HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);

// Toggle BUZZER
HAL_GPIO_TogglePin(BUZZER_GPIO_Port,BUZZER_Pin);
```
- It can be used to show state / debug, similar to leds
- Please do not *destroy* other's ears ^ ^

***Classwork 2** Distributed during tutorial*

#### Button Example and Classwork

```C
// Initialising a button
HAL_GPIO_InitPin(BUTTON1_GPIO_Port,BUTTON1_GPIO_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
// Remember the onboard button must be used with a pull up resistor

uint8_t pressed = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port,BUTTON1_GPIO_Pin);
// Remember the onboard button will give a low signal when clicked, and a high signal otherwise
```

#### Edge vs Level Triggering

Consider 2 uses for a button:
- While the button is down play a sound, while it is not, flash the LED
    - In this case every time the loop comes around, we are concerned with the current state (or level) of the buttons GPIO Pin
    - The implementation of the button reading here should be obvious and simple
- What if, we wanted to play a sound when the button is first clicked, but only once, so holding the button does nothing more, and when the button is first released, we want to flash the LED, but again only once
    - This is called **Edge triggering**
    - The event of a signal going from low to high is called the *rising edge* and the opposite is the *falling edge*
    - The GPIO_ReadPin function gives us the current state, but edge triggering also requires knowledge of the past state as well as some logic

***Thinking Time***: Who can think of how we can design some code that can call a function *only* when the button is first clicked?

```C
if (this_ticks - last_ticks >= 50) {  //This code runs every 50ms
    last_ticks = this_ticks;
    //Static ensures that the value of this variable persists through every iteration of the loop
    static uint8_t last_state = 0;
    uint8_t current_state = HAL_GPIO_ReadPin(...);
    if (last_state == 1 // Previously unclicked
        && current_state == 0 // Now clicked
        ) {
        buttonClicked();    
    } else if (last_state == 0 // Previously clicked
        && current_state == 1 // Now unclicked
        ) {
        buttonReleased();    
    }
    last_state = current_state; // Remember the button state for next loop iteration
}
```

***Classwork 2** Distributed during tutorial*

## Extra

<!--
███████ ███    ██ ██    ██ ███    ███
██      ████   ██ ██    ██ ████  ████
█████   ██ ██  ██ ██    ██ ██ ████ ██
██      ██  ██ ██ ██    ██ ██  ██  ██
███████ ██   ████  ██████  ██      ██
-->


### Enum
An **enumeration** is a type that can hold a finite set of symbolic objects, which are represented as integers internally. By default, the first object is given the value `0`, then the subsequent object is assigned a value **one greater** than the previous object's value. It is very useful in writing libraries as it increases the **readability** of the program, so everyone can understand the program easily.

Syntax of define a new enum:
```
typedef enum {<identifier1> [=value1], <identifier2> [=value2], ...} <new_datatype>;
```

The following is an example of `enum`:
``` C
typedef enum {
	BUTTON1,  // BUTTON1 = 0
	BUTTON2,  // BUTTON2 = 1
	BUTTON3,  // BUTTON3 = 2
} Button;

// define a variable with the new enum
Button btn = BUTTON1;
// assign a new value to variable "btn"
btn = BUTTON2;
// assign a new value by integer
// remember to cast it into Button type, otherwise warning will be given while compiling, but the program can still be compiled
btn = (LED_ID) 2;  // equivalent to btn = BUTTON3;
```

Alternatively, you can assign an integer value to the object in enum:
``` C
typedef enum {
	BUTTON1 = 1,  // BUTTON1 = 1
	BUTTON2,      // BUTTON2 = 2
	BUTTON3,      // BUTTON3 = 3
} Button;
```

<!--
███████ ████████ ██████  ██    ██  ██████ ████████
██         ██    ██   ██ ██    ██ ██         ██
███████    ██    ██████  ██    ██ ██         ██
     ██    ██    ██   ██ ██    ██ ██         ██
███████    ██    ██   ██  ██████   ██████    ██
-->

### Struct
A **Structure** is a collection of different kinds of objects. You can define a new user-defined data type by using `struct`. `struct` is more convenient than defining multiple variables.

Syntax of define a new `struct`:
```
typedef struct {
	<data_type> <identifier for the 1st member>;
	<data_type> <identifier for the 2nd member>;
	...
} <struct identifier>;
```

The following is an example of defining a 3D coordinate without struct:
```C
// define all variables
uint8_t x = 0;
uint8_t y = 0;
uint8_t z = 0;
// assign new values to variables
x = 1;
y = 2;
z = 3;
```

The following is an example of defining a 3D coordinate with struct:
```C
// define a new struct "Coordinate"
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
} Coordinate;
// define and initialize a struct variable
Coordinate coor = {0, 0, 0};  // {x, y, z}
// assign new values to variables
coor.x = 1;
coor.y = 2;
coor.z = 3;
```

<!--
██████  ███████ ███████ ██ ███    ██ ███████
██   ██ ██      ██      ██ ████   ██ ██
██   ██ █████   █████   ██ ██ ██  ██ █████
██   ██ ██      ██      ██ ██  ██ ██ ██
██████  ███████ ██      ██ ██   ████ ███████
-->

### Define
Similar to `const`, `#define` is used to create a **constant** in program, but using `#define` is different from using `const`. When using `#define`, things defined by `#define` are replaced to be the value before compilation. When using `const`, the value is still being stored inside a variable. Using `#define` can increase the readability of code and increase the ease of changing value.

Syntax of `#define`:
```
#define NAME <value>
```

The following is an example of using `#define`:
```C
#define DefaultLED 1
uint8_t led = DefaultLED;

// The above code is equivalent to the following:
uint8_t led = 1;
```

Note: `#define` can also be used to define macro function, but we won't discuss that in here.

### Finite State Machine (FSM)

When designing a robots program, or many other systems in fact, it is useful to think of the program as having some finite number of states such that each state has different actions to perform, each state has different conditions for moving to a new state and (optionally) while changing states a certain action is performed. This is useful as it is a very structured way of thinking, can be drawn out in a flowchart and there is a very straightforward implementation pattern: the switch statement

Example:
```C
//An enum will allow you to name your cases in a more readable way
typedef enum{
    FORWARD=0,
    BACKWARD,
    HALT
}RobotState;

static RobotState currentState = HALT;

while(1){

    //......
    //Your finite state machine
    switch(currentState){
        case FORWARD:
        {
            //instructions to execute while at forward-state
	    if (hitARoadBlock()) { //A conditional state transition
	    	state = BACKWARD;
	    }
            break;
        }
        case BACKWARD:
        {
            //instructions to execute while at backward-state
            break;
        }
        case HALT:
        {
            //instructions to execute while at halt-state
	    if (userSaysGoGoGo()) {
	    	state = FORWARD;
		//Optionally do something
	    }
            break;
        }

    }
}
```

The switch statement is not the only way of implementing it, but any way you do it, the structured thinking you do when designing a state machine is valuable. As an exercise try and think how the edge triggering problem from before can be formulated as a finite state machine

<br>

## Homework
release during your tutorial section via email
