Author: Ho Yu Yat (yyhoai@connect.ust.hk) (2019)

Author of Code: Anshuman Medhi (amedhi@connect.ust.hk) (2019)

# Homework 1 Solution

The code is in the file `homework.c`.

This code might look intimidating at first, but it is actually not hard to understand. It does have a slightly complex reference structure (i.e. many functions calling each other), a simple way to understand it is just to follow it through step by step.

Other than that, reading the explanations for some more advanced/unsual tricks in here should help you understand completely.

## Trying out the code yourself

Just make a header file like so and include it both in `main.c` and `homework.c`.
```c
//homework.h
#ifndef HOMEWORK_H
#define HOMEWORK_H

void main_loop(void);

#endif
```

Initialise the GPIO pins as needed.
```c
HAL_GPIO_InitPin(BUTTON1_GPIO_Port,BUTTON1_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
HAL_GPIO_InitPin(BUTTON2_GPIO_Port,BUTTON2_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
HAL_GPIO_InitPin(BUTTON3_GPIO_Port,BUTTON3_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
HAL_GPIO_InitPin(LED1_GPIO_Port,LED1_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
HAL_GPIO_InitPin(LED2_GPIO_Port,LED2_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
HAL_GPIO_InitPin(LED3_GPIO_Port,LED3_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
HAL_GPIO_InitPin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
```

Then just write the `while (1)` loop in `main()` like so:
```c
while (1) main_loop();
```

## What is a typedef enum?

From line 14-21 you'll see this:

```c
typedef enum {
    BUTTON1=0,
    BUTTON2,
    BUTTON3,
    //This trick means the NumButtons will equal how many buttons there are (how many enum values)
    //This works as long as the enum values are simple (count up from 0)
    NumButtons,
} Button;
```

An enum just means it assigns names to consecutive integers. Internally, the value is stored as an integer, but you can use a name to represent that value in the code. In the snippet above, the number `0` is given the name `BUTTON1`, then the subsequent names are just in order: `1`, `2`, `3`. `NumButtons`, the 4th name, will get the number `3`, which happens to be the number of buttons.

If the first name is defined as `BUTTON1 = 1`, then the enum will count starting from 1, so the numbers would be `1`, `2`, `3`, `4`. The `NumButtons` trick will not work anymore.

`typedef` is actually not part of the enum definition, it is another syntax in C that will give an alias to a data type. In the case, it gives `enum {...}` the name `Button`. Then all the subsequent code can declare a variable with the type `Button`, which will have 4 possible values.

It is also possible to directly give a name to the enum like so:
```c
enum Button {
    BUTTON1=0,
    BUTTON2,
    BUTTON3,
    //This trick means the NumButtons will equal how many buttons there are (how many enum values)
    //This works as long as the enum values are simple (count up from 0)
    NumButtons,
};
```
But then you have to refer to this datatype as `enum Button` everywhere else, so most people prefer `typedef`, as you only have to type `Button` to refer to the enum.

## What is 0xFFFFFFFF?

If you are wondering what this is on line 94
```c
#define TIME_NEVER 0xFFFFFFFF //max of uint32_t
```
Whenever you add the prefix `0x` before an integer literal, this literal will be in hexadecimal format. 0xFFFFFFFF is just 2<sup>32</sup> or 16<sup>8</sup>.

Just FYI, if for some reason you want a number in octal (base 8), just add a `0` in front of the literal. So be careful, `0100` is different from `100` in C. :D

## What is a typedef struct?

It's the exact same reason why typedef enum exist.

## What does the static keyword do in functions?

Simply speaking, identifiers with the `static` keyword behave like a global identifier, but it can only be accessed in the scope it is declared in. This might sound contradictory "What is it supposed to mean it's global but it's local???", allow me to explain.

Global identifiers, once constructed, will only be destructed at the very end of the entire program execution. This is also the case for identifiers with the `static` keyword. This is what I mean when I say they behave like global identifiers.

For example:
```c
int staic_demo() {
    static int i = 0;
    return ++i;
}
```

The integer `i` will be constructed as soon as the function is defined, while normally it will be constructed each time the function is called. And normally `i` will be destructed as soon as the function returns, but now `i` will retain its value even when the function is not running.

In effect, if the `static` keyword is removed, then the function will return `1` every time. But now, the first function call will return `1`, the second function call will return `2`, so on and so forth.

Technically, it is possible to do this to achieve the same effect
```c
int i = 0;
int bad_demo() {
    return ++i;
}
```
But this is not desirable, as the name `i` is in global scope, it could easily conflict with another `i` declared elsewhere.

## Is there a difference when adding the static keyword to a global variable?

Yes. For example, if you have the global variable
```c
int i = 0;
```

Other .c files will be able to access `i` if they add this line, as it is in the global scope
```c
extern int i;
```

But if the variable has the `static` keyword
```c
static int j = 0;
```

Then other files cannot access `j` even if they `extern` it, as it is in the file scope (i.e. only accessible in the file it is declared in).

## What are the operators & and <<?

You may have seen these operators from this function on line 236
```c
uint8_t num_get_bit(uint32_t num, uint8_t bit) {
    //There are many ways to do this, ranging in complexity
    //This uses bitwise and and lshift
    return (num & (1<<bit)) != 0; 
    //(1<<bit) shifts the number 1 leftward by 'bit' positions
    //thus we get a number that has its 'bit'th position set
    //eg 1<<3 = 0b1000
    //eg 1<<9 = 0b0001
    //etc
    //num & mask takes every bit from 'num' and does logical AND with the corresponding bit in 'mask'
    //practically the effect is that 'mask' acts like a gate, 
    //the '1's in mask allowing the corresponding bit from num to pass
    //the '0's in mask 'block' the bit from num and replace it with a zero
    //if mask has only 1 bit set (like 1<<bit) then (num&(1<<bit)) clears all bits except for one of them
    //thus if the result is 0 then that bit was not set
    //and if the result is nonzero that bit must have been set
}
```

First, understand that this function is to extract a specific bit from a binary number. It labels the right-most bit as bit 0, then the one to the left is bit 1, so on and so forth.

`<<` is bitwise left shift. e.g. You have the binary number `0010`, if you do `0010 << 1`, then the number is shifted 1 bit to the left to form `0100`.

`&` is bitwise AND, it performs the AND operation on each bit of 2 binary numbers. e.g. You have 2 binary numbers `0110` and `0011`. If you do `0110 & 0011`, bit 0 is `0` AND `1` = `0`, bit 1 is `1` AND `1` = `1`... This expression will return `0010`, as the bits are both `1` only on bit 1.

To understand `(num & (1<<bit)) != 0`, let's look at an example. `num` is `0110` and you want bit 2.
1. `1<<2` will evaluate as `100`.
2. `num & (1<<bit)` = `0100 & 100` = `0100`. The missing bit in `100` is assumed to be `0` (`100 == 0100`).
3. Now bit 2 is extracted and you can clear see it's `1` from `0100`. But you need to somehow return `1` instead of `0100`. So you apply a little trick and do `0100 != 0` = `1`.

*Note that the binary numbers in the steps are not formal C expressions, they are only for illustration.*
