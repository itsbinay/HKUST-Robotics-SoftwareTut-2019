# Advanced Tutorial 1 - UART - TFT
## TFT
>By Binay Gurung

When learning programming, most of the integrated development environment (IDE) you will/might have used will have a console for output and will often be used for debugging for you to keep track of certain variables and to trace-out truth tables or so forth. In different, programming languages, you can print/show your variable's value in the program using print function.

###### For Example
In C,
```C
int c = 25;
printf("The value of c-squared is : %d",c*c);
//Output will come as "The value of c-squared is : 625"
```
In Python,
```Python
c = 25
print("The value of c-squared is: ",c*c)
#Output will come as "The value of c-squared is : 625"
```
In Java,
```Java
int c = 25;
System.out.println("The value of c-squared is ",c*c);
//Output will come as "The value of c-squared is : 625"
```

However, in our embedded system, usually we do not have a console to output our variable values to when debugging. However, with the help of TFT (a small LCD monitor), you will be able to **"print out"** the values of your variable on a **monitor**.
### Initialize TFT
###### Function Prototype
```C
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp,uint16_t in_highlight_color);
```
###### Parameters
- orientation - ***Orientation of the monitor***
- in_bg_color - ***Background color***
- in_text_color - ***Text color***
- in_text_color_sp - ***Special Text color***
- in_higlight_color - ***Highlight color***

The parameters have already been defined for you in `lcd.h` header-file. It is defined as follows:
######  Orientation
```C
typedef enum {
    PIN_ON_TOP, 
    PIN_ON_LEFT, 
    PIN_ON_BOTTOM, 
    PIN_ON_RIGHT
} TFT_ORIENTATION;
```
######  Colours
You may choose one of the following colours according to your own desire for the **TFT**.
```C
#define WHITE           (RGB888TO565(0xFFFFFF))
#define BLACK           (RGB888TO565(0x000000))
#define DARK_GREY       (RGB888TO565(0x555555))
#define GREY            (RGB888TO565(0xAAAAAA))
#define RED             (RGB888TO565(0xFF0000))
#define DARK_RED        (RGB888TO565(0x800000))
#define ORANGE          (RGB888TO565(0xFF9900))
#define YELLOW          (RGB888TO565(0xFFFF00))
#define GREEN           (RGB888TO565(0x00FF00))
#define DARK_GREEN      (RGB888TO565(0x00CC00))
#define BLUE            (RGB888TO565(0x0000FF))
#define BLUE2           (RGB888TO565(0x202060))
#define SKY_BLUE        (RGB888TO565(0x11CFFF))
#define CYAN            (RGB888TO565(0x8888FF))
#define PURPLE          (RGB888TO565(0x00AAAA))
#define PINK            (RGB888TO565(0xC71585))
#define GRAYSCALE(S)         (2113*S)
```
###### Example
```C
/* 
## Initialisation Example ##
Orientation : Pin_on_top
Background color : black
Text color : white
Special Text color : red
Highlight color : green
*/
tft_init(PIN_ON_TOP,BLACK,WHITE,RED,GREEN);
```
##### Print String
```C
void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...);
```
- **x**: nth horizontal column ranging from 0 to 15 (16 columns)
- **y**: nth vertical row, ranging from 0 to 9 (10 rows)
- **pstr**: string (same as C's printf)
- **...** : variable to replace the placeholder in the string ( same as C's printf)

###### Example
```C
int a = 10;
tft_prints(0,0,"The value of a is : %d",a);
```
##### Print Pixel
```C
void tft_print_pixel(uint16_t color, uint32_t x, uint32_t y);
```
- **color** : colour of your pixel (Use the #define colours)
- **x** : n-th horizontal pixel, ranging from 0 to 127 
- **y** : n-th vertical pixel , ranging from 0 to 159

##### Update
```C
uint8_t tft_update(uint32_t period);
uint8_t tft_update2(uint32_t period);
```
- **period** : period of update in ms

### Example of using TFT
```C
while(1){  
    /*This is referring to your main while(1) loop, 
      Do not create another while(1)*/
    if(tft_update(50)==0){
        tft_prints(0,0,"Hello World");
    }
}
```
## Universal Asynchronous Receiver-Transmitter (UART)
> By Jason Leong
> Updated by Binay Gurung

Universal Asynchronous Receiver-Transmitter (UART), is a protocol used for serial communication and is used when the rate of transmission is not a concern. For example, uart could be used to send control commands but it isn't not suitable to send high resolution images.

In our embedded system, UART may commonly applied when establishing a communication protocol between two different boards or when trying to send data from an mobile/desktop application to your board using bluetooth. 

### Sending and receiving data with UART
UART(Universal asynchronous receiver-transmitter) sends your data in serial in asynchronous mode. Serial communication is the process of sending data one bit at a time, and asynchronous means that there is no common clock signal between the sender and receivers. (or in another word, the clock of STM32 and the Bluetooth device may be different, this is why you need to set the baud rate on both sides)

### Initialization of your UART Port
On your STM32 board, there are two UART ports available for you to use for your internal communication according to your preference.

Like most board/pins controls you have learnt so far, a UART port needs to be initialised before it can be used.

###### Function Prototype
```C
void USART1_Init(uint32_t baudrate); //To initialise UART 1
void USART3_Init(uint32_t baudrate); //To initialise UART 3
```

###### Baud Rate
As we know that UART is an asynchronous protocol (no clock), it is difficult for either side of a communication network to identify the data. In order for it to work, both the receiver and transmitter need to have the same Baud Rate. If it has the same baud rate, it would know exactly where the bits/data being sent are being clocked. The Baud rate is related to the speed of data transmission in a system. The rate indicates the number of electrical oscillations per second that occurs within a data transmission. The higher the baud rate, the more bits per second that are transferred.

Baud Rates have a predefine standards and the baudrate we usually use are as follows
- 9600
- 38400
- 57600
- 115200 
With 57600 and 115200 being the most common baud rates. **Make sure the baud rates on both side of the communication is the same.**
###### Example
```C
//The following line of code initialises UART port 1 with a baudrate of 57600
USART1_Init(57600);
```
### Receiving data on your STM32
To receive data on your STM32, we will use the function `HAL_UART_Receive`.

###### Function Prototype
```C
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
```

###### Parameters
- ***huart*** - indicate which uart port (either huart1 or huart3, defined at the top of usart.h)
- ***pData*** - data buffer (i.e. variable where the received data will be stored)
- ***Size*** - Amount of data to be received (in bytes)
- ***Timeout*** - Timeout duration in ms

##### Example of receiving data
``` C
int main(){
    //....Initialisation stuffs
    USART1_Init(115200);    //Uart 1 initialised with 115200 baud rate

    //Initialising your buffer variable
    char data[8];

    while(1){
        /*
        Receive data from uart 1,
        store it in the data buffer "data"-variable,
        */
        HAL_UART_Receive(&huart1,data,8,10);
    }
}
```
### Sending Data from your computer to STM32

We use Coolterm for demonstration, but there are different ways to interact UART signal. Examples are Tera term, mobile application, and python/C# program written by yourself using Bluetooth stack. However, we may not be able to provide support to people not using Coolterm.

##### Connecting STM32 to the computer via USB-TTL
Locate the UART1/UART3 port on your STM32 (its below your ST-Link port/flashing port) and connect your TTL to the UART port as follows.

|TTL | Uart Port|
|---|---|
|`5V0`|`V`|
|`TXD`|`R`|
|`RXD`|`T`|
|`GND`|`G`|

###### Setting up and using CoolTerm
Open coolterm and set up as below:
```
**option -> serial port-> port = port you used to connect your TTL to your computer
serial port -> baud rate = 115200
serial port -> data bits = 8 bit
serial port -> parity = none
serial port -> stop bits = 1**

**terminal -> termianl mode = line/raw mode 
terminal -> Enter key emulation = CR/LF
terminal -> local echo = check
Press connect and run your board, and the messages should appears.**
```
###### Explanation:
Baud rate basically means the number of bits of the signal sent per second. 115200 is basically the quickest speed that can works on most devices without any problem. For example, standard windows (or, .net) utilities does not support anything above 115200.
According to the standard of uart, it sends 1 start bit (must), 5/6/7/8 bits of data, 1 bit of parity check (optional),1/1.5/2 bit of stop bit. Our setting uses 1 start bit, 8 data bits, 0 parity bit and 1 stop bit, which means that 8 bit = 1 byte out of 10 bits sent are data, with a baud rate of 115200, effectively 11520 bytes of data could be sent in one second.
**The two extra bit of start bit and stop bit are handled automatically by stm32 and coolterm and you don't have to worry about them.**

For the coolTerm terminal, raw mode means that data will be sent when you press a key on the keyboard, Whereas there will be an extra line for you to input a whole string and sed at once in line-mode. This two modes mostly matter when configuring the Bluetooth device and will be covered later.

Local echo means that cool term will display what you typed and does not affect what you send/receive.

### Sending data from STM32 to your computer
Other than for receiving data from another device on your board, you can also transmit data to other devices. This can come in handy in many ways for example, you could use coolTerm as your console for printing out the values while debugging and so forth.

To send data from STM32,we can use the `HAL_UART_Transmit` function.

###### Function prototype
```C
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
```
###### Parameters
- ***huart*** - indicate which uart port (either huart1 or huart3, defined at the top of usart.h) you are sending from
- ***pData*** - data buffer (i.e. variable where the sent data will be stored)
- ***Size*** - Amount of data to send (in bytes)
- ***Timeout*** - Timeout duration in ms

##### Example of sending data
``` C
int main(){
    //....Initialisation stuffs
    USART1_Init(115200);    //Uart 1 initialised with 115200 baud rate

    //Initialising your buffer variable
    char data[8]="Hello";

    while(1){
        /*
        send data from uart 1,
        send the data in the "data"-variable,
        */
        HAL_UART_Transmit(&huart1,data,8,10);
    }
}
```
### Setup Your Bluetooth device
>This process may or may not have done for you, so it may be a better idea to do it by yourself

>Technically all uart port can be used to config the Bluetooth, including the stm32, but using Coolterm is just the quicker way.

>The way of configuring HC05 is provided, and other devices such as HC06, HM10 may be different
1. Connect USB-TTL with your device as follows: 
   
   |USB-TTL|Bluetooth|
   |---|---|
   |`5V0`|`5V`|
   |`GND` |`GND`|
   |`TX` | `RX`| 
   |`RX` |`TX`|
2. Hold the button on HC05 while plugging in the USB to the computer
3. Release the button, HC05 should be in "AT" mode and the led should flash slowly.
4. Set coolterm baud rate to 38400 and connect.
5. Type "AT" and press enter. If it replies "OK", then you can use the following AT commands to configure the device. Otherwise, check the steps above again.
6. Note that some commands (such as AT+NAME=.../AT+NAME?) may require you to hold the button while pressing enter to work.
7. Note that AT command is case-sensitive

##### List of commands:

1. AT (for Testing)
2. AT+RESET (back to normal mode, doesn't mean reset configs)
3. AT+NAME? (return name of the device)
4. AT+NAME=<Param>  (set name)
5. AT+ PSWD?  (return password)
6. AT+ PSWD=<Param>  (set password)
7. AT+UART=<Param>,<Param2>,<Param3> 
(set baud rate,stop bit (0 -- 1 bit,1 -- 2 bit),no. of parity bit)
recommended settings: AT+UART=115200,0,0
8. AT+UART?  (return setting of uart)
9. AT+ORGL  (reset all setting to default ones)

After configuring the device, you may plug it onto UART port of your STM32 board. The computer should be able to find the device when power is given to HC05. After pairing with the device, rescan the port in the "serial port" option of coolterm and new ports should appear. The port that requires time connecting is likely the correct port, as your computer is acting as the master side and need to wait for HC05 to respond, while the other port waits for HC05 actively connect with the computer(which will not happen).

Adding in HC05 allow you to communicate wirelessly, but the actual usage is the same with connecting directly. 