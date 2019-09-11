Author: Daniel Cheung (dcheungaa@connect.ust.hk)

Updated and expanded by by Ho Yu Yat (yyhoai@connect.ust.hk)

##### Submissions and questions:
Send to Ho Yu Yat (yyhoai@connect.ust.hk) before 15/9 23:59pm.

Zip the .c and .h files in each task into separate zip files unless otherwise specified in the task.
e.g. calculator.zip + sin.zip + ab.zip

# Homework

This homework aims to check your understanding in the syntax of C programming, as well as to test the flexibility of your mind, to see whether you can come up with ingenious solutions to the problems. Simpler and faster code is always better.

For the following problems, there are no code skeletons (except bonus). As long as your code is clear, it will be marked fine. You may ask seniors for any queries and Google if you need to. Have fun doing the homework!

## Simple Calculator

Implement a simple calculator program that:

1. Ask for the mode of operation by reading a character input.
2. Ask for the first int.
3. Ask for the second int.
4. Display the result in form of `<first_number> <operator> <second_number> = <result>`
5. Ask whether they want to try again by reading a character. If they reply `'y'`, repeat the program. Otherwise, end the program.

Assume all inputs are correct.

| Operations     | Character |
| -------------- | --------- |
| Addition       |     `'+'` |
| Subtraction    |     `'-'` |
| Multiplication |     `'*'` |
| Division       |     `'/'` |
| Modulus        |     `'%'` |

#### Expected combined console inputs and outputs

```
Select a mode of operation (+, -, *, /, %): -
First number: 1
Second number: 2
1 - 2 = -1
Again? y
Select a mode of operation (+, -, *, /, %): +
First number:  1
Second number:  2
1 + 2 = 3
Again? n
```

When writing the format string for `scanf() ` to read a character, use this: `" %c"`. Because pressing the Enter key counts as inputing a newline character (`'\n'`). Entering a trivial character as well as the Enter key will count as 2 characters. Since this format string `"%c"` only consumes 1 character, the Enter key would be put in a buffer for until the next time `scanf()` is needed. In other words, the `'\n'` would be queued for the next time when `scanf()` reads a character, and would be consumed immediately, so the console will not pause and ask for an input. The space in the correct format string will consume the Enter keys, effectively ignoring them.

## Mesmerizing Sin Curve

In this problem, you have to draw out a vertical version of the mathematical sin curve in the console with spaces and asterisks using loops. The following required facts are provided to you.

- To use mathematical functions like `sin()`, include the header `<math.h>`.

- The `sin()` function has the following prototype, from [cppreference](https://en.cppreference.com/w/c/numeric/math/sin):

  ```c
  double sin( double arg );
  ```

  `arg` is a double storing the angle in radian units.

- `<math.h>` includes a macro called `M_PI` which translates to the value of pi. But since it is not part of the C standard, you may have to add the line `#define _USE_MATH_DEFINES` before including `<math.h>` to use it.

You must initialize variables to store the following. The program should work as intended when these values are changed, compiled and run again.

- `periods` - How many periods of the sin curve are drawn;
- `x_size` - How wide the sin curve is printed be;
- `y_size` - How tall a period of the curve is drawn.

#### Expected parameters and outputs

When:

- `periods` is 3
- `x_size` is 20
- `y_size` is 15

### Corrected output:

The console shows:

```
          *
              *
                 *
                    *
                    *
                   *
                *
            *
        *
    *
 *
*
*
   *
      *
          *
              *
                 *
                    *
                    *
                   *
                *
            *
        *
    *
 *
*
*
   *
      *
          *
              *
                 *
                    *
                    *
                   *
                *
            *
        *
    *
 *
*
*
   *
      *

```

Notice at the smallest output of the sin function, the x-position of the `*` is at 0, and at the largest output, the x-position is at 20.

## A and B

Initialized with an array of `'a'`s and `'b'`s like so `{'a', 'b', 'b', 'a', 'a'}`. Whenever you find 2 adjacent  `'a'` and `'b'`, remove them. Repeat that until you cannot find pairs anymore, then print out the array to the console.

#### Sample inputs and outputs

- **Sample 1**

  Input

  ```c
  {'a', 'b', 'a'}
  ```

  Console output

  ```
  a
  ```

- **Sample 2**

  Input

  ```c
  {'a', 'b', 'b', 'a'}
  ```

  Console output

  ```
  
  ```
  
## Number of inner parameter (BONUS)

#### Instruction

- A valid parameter start with `(` and end with `)`. A inner parameter is a valid parameter wrapped by a valid parameter.Given a valid parameter eg: `(())`, there are 2 parenthesis, 1 outer parameter and  1 inner parameter, then the number of inner parameter is `1`.

```c
//Sample main.c
#include <stdio.h>
#include <stdbool.h>
#include "count.h"

int main() {
    char input[] = {'(','(','(',')',')',')','(',')','(',')','(','(',')',')','(','(',')',')'};
    printf("%d",CountInnerParameter(input,sizeof(input)/sizeof(char))); //Expected output is 4
    return 0;
}
```

```c
//count.h

//in: input char array, len: length of char array
int CountInnerParameter(char in[], int len) {
    //Complete the function
}
```

- Write the function `CountInnerParameter` in a file called `count.h` (no need separate .c file).
- Send the `count.h` file directly as submission, don't need to zip.

#### Expected output
test case 1: input charcter array: `"()"` output:`0` <br/>
Explain: There is only 1 valid parameter. 0 inner parameter.<br/><br/>

test case 2: input charcter array: `"((()))()()(())(())"` output:`4` <br/>
Explain: The input can be split into 4 sub-problems: How many inner parameter in `"((()))"` and `"()"` and `"()"` and `"(())"` and `"(())"`<br/>
The number of inner parameter in `"((()))"` is 2.<br/>
The number of inner parameter in `"()"` is 0.<br/>
The number of inner parameter in `"()"` is 0.<br/>
The number of inner parameter in `"(())"` is 1.<br/>
The number of inner parameter in `"(())"` is 1.<br/>
