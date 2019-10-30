# Camera and Image Processing

Author : Amrutavarsh Kinagi

Contact: askinagi@connect.ust.hk 

Author : Morris Tseng

Contact: mtseng@connect.ust.hk 

Updated by: Ho Yu Yat (yyhoai@connect.ust.hk) (2019)

## Camera - Ov7725

Ov7725 is will be the camera modules you will be using during the Robot Design Contest. It can output greyscale and RGB565.

The camera configuration is passed by a communication protocol called SCCB (similar to I2C). It is handled by the camera library.

Due to the limited memory size and computation power of STM32F1, the default image size is set to be 80 * 60.

You may have to implement your own method of processing the image data.

## Setting Up

All functions with the `CAM_` prefix are defined in `camera.h`.

### Camera Initialization 

Initialize the camera.

```c
CameraStatus CAM_Init(void); //Init

uint8_t CAM_Initialized(void); //Check if init
```

Where `CameraStatus` is an enum that contains RGB565 and greyscale, also defined in `camera.h`.

### Colour Mode

```c
void CAM_SetColorMode(CameraColorMode c); //Set colour mode

CameraColorMode CAM_GetColorMode(void); //Read colour mode
```

Where `CameraColorMode` is an enum containing the option for RGB565 and greyscale, also defined in `camera.h`.

#### Greyscale

Each pixel occupies 1 byte, value ranges from 0-255.

#### RGB565

Each pixel occupies 2 bytes, the bits are split into 3 groups to represent RGB respectively.

- R: Occupies 5 bits, so its value range from `00000` to `11111`.
- G: Occupies 6 bits, so its value range from `000000` to `111111`.
- B: Occupies 5 bits, so its value range from `00000` to `11111`.

Concatenate the 3 values in the order R, G, B. So if R=`11011`, G=`1100101` and B=`11010`, then the final RGB565 value is `11011110010111010`.

### Frame Size

```c
void CAM_SetFrameSize(CameraFrameSize f); //Set frame size

uint32_t CAM_FrameWidth(void); //Read current frame size
uint32_t CAM_FrameHeight(void);
```

Where `CameraFrameSize` is an enum containing various preset frame size, also defined in `camera.h`. Defaulted to 80x60.

### Brightness and Constrast

```c
void CAM_SetBrightness(uint8_t brightness);
void CAM_SetContrast(uint8_t contrast);
```

You may want to tune this to suit the lighting environment.

### Setting Frame Rate

```c
void CAM_SetFrameRate(CameraFrameRate r);
```

Where `CameraFrameRate` is an enum containing various preset fps values, also defined in `camera.h`. Defaulted to 75fps.

## Usage

### 1. Check whether the image is ready to receive in external buffer.

```C
uint8_t CAM_FrameReady(void);
```

### 2. Receive image from the external buffer.

```C
void CAM_GetGrayscale(uint8_t* ptr);
void CAM_GetRGB565(uint16_t* ptr);
```

You need to define an array of the appropriate size to store the image data and pass into these functions. e.g.

```c
//Greyscale
uint8_t grey_option1[CAM_FrameHeight()][CAM_FrameWidth()];
uint8_t grey_option2[CAM_FrameHeight() * CAM_FrameWidth()];

//RGB565
uint16_t rgb_option1[CAM_FrameHeight()][CAM_FrameWidth()];
uint16_t rgb_option2[CAM_FrameHeight() * CAM_FrameWidth()];

//Example usage
CAM_GetGrayscale(grey_option1);
CAM_GetRGB565(rgb_option1);
```

#### Pointers

You may be wondering what is `uint8_t*`, it is what's called a pointer variable. Any variable that has a `*` after the data type is a pointer variable.

If you want to access the value stored in the memory address, you just need to add a `*` before the pointer variable, this action is called *dereferencing*. e.g.

```c
uint8_t i = 2;
uint8_t* iptr = &i;

*iptr = 4; //The value of i is now 4
```

Pointers are variables that contain a memory address, i.e. a location in RAM. The unit of memory addresses are bytes.

Memory addresses are typically expressed as hexidecimal numbers, e.g. `0x00000001` (recall an integer literal with the `0x` prefix is a hex number).

The program assumes the size of the variable that is occupying the memory address in a pointer to be the size of the data type.
- e.g. The program sees a `uint8_t*` pointer and knows the byte is occupied.
- e.g. A `uint32_t*` pointer means the variable pointed by the pointer occupies 4 bytes (4 bytes * 8 bits/byte = 32 bits).
- Therefore be very careful when casting between pointer types, e.g. The program doesn't know the variable actually occupies 4 bytes if you cast a `uint32_t*` to `uint8_t*`.

Pointer variables support addition and subtraction. The address will be incremented by the value you added to to address multiplied by the size of the data type. If this sounds confusing to you, consider the following demonstration.

```c
uint32_t i = 1;
uint32_t* iptr = &i; //0x00000001 for example

iptr = iptr + 3; //iptr is now 1 + (32 / 8) * 3 = 0x0000000D (13)
iptr = iptr - 2; //iptr is now 13 - (32 / 8) * 2 = 0x00000005 (5)

iptr++;
iptr--; //These work too
```

**This is just an example, don't do this for real in your program!!** Your program probably does not occupy the address `0x0000000D`, if you point a pointer to it and tries to modify the value via the pointer, you will mess up the calculations in other processes, assuming the operating system has no protection against this at all. In reality, on most systems, trying to modify a memory address not allocated to your program will terminate the program with a segmentation fault as a protective measure.

The practical use of this mechanism will be explained a bit later.

#### Multi-dimensional Arrays

You should know what is an array by now.

Multi-dimensional arrays in C means "Array of Arrays". e.g. `rgb_option1` is an array containing `CAM_FrameHeight()` arrays that contain `CAM_FrameWidth()` `uint16_t` variables.

Syntax-wise, you can have unlimited dimensions, i.e. you can have 11D arrays if you want. But in reality, you may be very restricted by memory, depending on your system, so an array of this many dimensions are probably over the memory limit. *For those interested to learn more about this matter, research the difference between static memory allocation (stack) and dynamic memory allocation (heap). The typical variable uses static memory allocation, and thus occupies the stack. Then search about stack size limit of different systems.*

#### Passing Array into Pointer?? I'm Confused

Note the following trivia about C before reading the explanation.

- Memory storage of arrays are contiguous, meaning the elements are ordered by index and sticks to each other in the memory.
  - e.g. The size of each element of the `uint8_t example[3];` array is a byte, if `&example[0] == 0x00000000`, then `&example[1] == 0x00000001` and `&example[2] == 0x00000002`.
  - e.g. For `uint16_t example2[3]`, if `&example[0] == 0x00000000`, then `&example[1] == 0x00000002` and `&example[2] == 0x00000004`.
- For multi-dimensional arrays, intuitively it will store the inner-arrays contiguously.
  1. Consider `uint16_t array_2d[2][3]`. It is an array containing 2 `uint16_t` arrays.
  2. The size of each inner-array is `16 bits / 8 bits/byte * 3 = 6 bytes`.
  3. Therefore, if `&array_2d[0][0] == 0x00000000`, then `&array_2d[1][0] == 0x00000006` and `&array_2d[2][0] == 0x0000000C`.

**Key point**: An array type can be implcitly (i.e. no casting) converted to a pointer type. In a lot of cases, when you are referring to an array, you are actually using it as a pointer.

Some cases are demonstrated below:

```c
uint16_t array[3] = {1, 2, 3};

//This line of code
int i = array[1];

//Is actually an alias of this code
int i = *(array + 1); //array being added like a uint16_t pointer, and then dereferenced

//Therefore
if (array == &array[0]) printf("They are the same."); //They are the same
```
```c
CAM_GetGrayscale(grey_option1); //grey_option1 is being treated like a pointer
```

This also explains why both of these will work:
```c
uint8_t grey_option1[CAM_FrameHeight()][CAM_FrameWidth()];
uint8_t grey_option2[CAM_FrameHeight() * CAM_FrameWidth()];
```
Just try think of the order of their memory storage, these 2 arrays are actually stored identically in RAM.
In the `CAM_Grayscale()` function, it can just do `ptr + 1`, regardless of whether it is a 1D or 2D array.

### 3. Print image

The TFT header have this function to print the camera image on the screen. `x` and `y` is where the top-left of the image will be. You need to manually pass in the width `w` and height `h` of the image, you may use `CAM_FrameWidth()` and `CAM_FrameHeight()` here.

```c
void tft_print_image(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
```

**This function only supports Big-Endian RGB565 array**. Regardless you are using Greyscale or RGB565, since our processor is Little-Endian, you need to define another array and call the following functions to convert the image to a printable format. You will then pass the resulting array into `tft_print_image()`.

```c
void CAM_GreyToRGB565(uint8_t* img, uint16_t* i2); //Use this if you are using greyscale
void CAM_RGB565ToPrint(uint16_t* img, uint16_t* i2); //Use this if you are using RGB565
```

## Image Processing

Image Processing and the usage of magnetic sensors are two vital parts in the Smart Car sub-team and the racing robot in the internal competition.  Since is is mandatory that the car be fully automated, we will give you a quick and brief guide to it. 

### Before Image Processing

Since the camera image is stored in pixels in a 2d array, it is important to know the concept and implementations of arrays.

Here is a 2D array with width 5 and height 3

| [0][0] | [0][1] | [0][2] | [0][3] | [0][4] |
| ------ | ------ | ------ | ------ | ------ |
| [1][0] | [1][1] | [1][2] | [1][3] | [1][4] |
| [2][0] | [2][1] | [2][2] | [2][3] | [2][4] |

Below is a demonstration of a typical way to manipulate a 2D array:

```c
#include <stdio.h>
#define WIDTH 5
#define HEIGHT 3
void print_array(int A[][WIDTH], int height){
    for(int i = 0; i<height; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
}
int main() {
    int myArray[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++){
        for (int j=0; j < WIDTH; j++){
            myArray[i][j] = WIDTH * i + j;
        }
    }
    print_array(myArray, HEIGHT);
}
```

Output

```
0 1 2 3 4 
5 6 7 8 9 
10 11 12 13 14 
```



### Image Processing

#### 1. Image Type (Colored, Grayscale or Black&White)

**Colored**

<img width="480" alt="colored" src="https://user-images.githubusercontent.com/32810188/44947015-2f9d3180-ae39-11e8-854a-a32387a246ef.png">

**GrayScale**

<img width="480" alt="greyscale" src="https://user-images.githubusercontent.com/32810188/44947020-404da780-ae39-11e8-8c8b-f1113f03550c.png">

Please note that the images are rendered instead of taking in reality, so the image shown here may not fully represent the real image that you will be getting.

##### Pros and Cons

- Colored: (colors are stored as RGB in the MCU)
- Pros: Contains lot of details
- Cons: Hard to control
- GreyScale: (range from 0 ~ 255, which black = 0 and white = 255)  **Recommended**
- Pros: Easier to control(compared to Colored image), able to use gradient to differentiate features
- Cons: 
- Black & White: (represented by 1 and 0, which black = 1 and white = 0)
- Pros: Easiest to control
- Cons: Hard to adjust thresholding for the image, have minimal information

These pros and cons are just a means to guide you. However, these rules aren't set in stone. It is general that for image processing, we would usually choose greyscale or black&white images to analyze as they provide adequate information in order to handle the dynamics of the car. The use of Colored image may provide more information but it is upto you to determine if the extra information available is of any substantial benefit.

#### 2. Brightness & Intensity

Apart from color choices, the brightness and the intensity of the image captured also affect the quality of the image you obtain, which in turn affect the ability to extract good data from it.

**Low Intensity**

<img width="480" alt="lowintense" src="https://user-images.githubusercontent.com/32810188/44949429-b49d4080-ae63-11e8-902e-17e64f7caf88.png">

**Medium Intensity**

<img width="480" alt="mediumintense" src="https://user-images.githubusercontent.com/32810188/44949432-b961f480-ae63-11e8-8896-d919740a78bd.png">

**High Intensity**

<img width="480" alt="highintense" src="https://user-images.githubusercontent.com/32810188/44949434-bbc44e80-ae63-11e8-8e68-152058dd8f3a.png">

If you do not find the proper intensity, your image might contains noise that would deter you from processing the information accurately. It is important that the program you write is able to adjust the brightness and intensity based on different lighting conditions as it is against the rules to flash your program during the competition.

## Convolution of image

#### Introduce to kernel

A kernel is a square matrix that specifies spatial weights of various pixels in an image. Different image processing have different kernels.

<img width="480" alt="wrxle" src="https://user-images.githubusercontent.com/32810188/45767787-afb2fc00-bc6d-11e8-81a5-29ba95f1c94a.png">

#### Convolution Process

Convolution of a matrix involves laying a matrix over another and then calculating the weighted sum of all pixel values.In this example, it is using an 3x3 kernel for convolution. If the input image is a mxn array, then the ouput array should be (m-2)x(n-2).

<img width="480" height="360" src="https://user-images.githubusercontent.com/32810188/45766774-f6ebbd80-bc6a-11e8-9aed-c7f57fe57159.JPG">

105*(0)+102*(-1)+100*(0)+103*(0)... = 89

More examples:

<img width="480" height="360" src="https://user-images.githubusercontent.com/32810188/45768526-bb9fbd80-bc6f-11e8-9d93-120bb90f9175.gif">

## Application for convolution - Feature Dectection

Here are some images that have gone through the convolution.

![](https://i.imgur.com/T17jDCz.png)



Convolutions can extended to further detect other vital features of the track. Another key term to keep in mind is **Gradient**.




![](https://i.imgur.com/AbQLWej.png)



Usually image generated might contain noise and will have to be **filtered/ smoothened** to prevent it from affecting the image analysis. 



![](https://i.imgur.com/v9DjmbP.png)

Use of a median filter to improve an image severely corrupted by defective pixels



**Points to remember:**

1. Real life situations largely vary from digital replicated simulations. Application of algorithms should thus be geared to tackle the blemishes you would face.
2. Analyse features and algorithms from a mathematical and physical perspective. 
3. Keep the program robust in order to tackle the worst cases possible. 
4. Tuning the physical properties of the car is allowed and remains essential for ensuring the car runs according to plan.
5. As images are stored in the form of matrices, keep in mind the run time of your program during feature extraction or **path planning.**

### Further Pondering

- Applying convolution to the whole image is an processor-intensive task, and might produce a large amount of unnecessary information. How to extract only the needed information in your application?
- Or even better, only produce the needed information? (This will be crucial in our application)
