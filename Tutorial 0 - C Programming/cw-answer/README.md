Author: Ho Yu Yat (yyhoai@connect.ust.hk) (2019)

# Classwork 0 Answers

You may see some slightly unusual ways of using the for loop in some of the answers, just keep in mind that the following for loop
```c
for (int i = 0; i < 5; ++i) {
    printf("Test");
}
```
is the short form of the following code
```c
{
    int i = 0;
    while (i < 5) {
        printf("Test");
        ++i;
    }
}
```
then it should be easier to understand what my code is doing.

**Note that the whole code is enclosed in a compound statement `{}`, this is very important, as it implies that the everything in the foor loop is enclosed in a scope, which means that any code outside the for loop cannot access anything that is defined within the for loop.**

As usual, you are very welcome to send email to me if you have any further questions about these answers.

## 1. Modifying Hello World

```c
#include <stdio.h>

int main() {
    printf("Your Name.\n");
    return 0;
}
```

Very self-explanatory.

## 2. Swapping contents

```c
#include <stdio.h>

int main() {
    float cup_x = 3.4f;
    float cup_y = 2.7f;

    float temp = cup_x;
    cup_x = cup_y;
    cup_y = temp;

    printf("Cup X now contains %fml.\n", cup_x);
    printf("Cup Y now contains %fml.", cup_y);
    return 0;
}
```

A temporary variable `temp` is used to temporarily store the value of `cup_x`, then after `cup_x` got assigned the value of `cup_y`, the original value of `cup_x` is assigned to `cup_y` via the use of `temp`.

## 3. Discount Eligibility

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    int age = 45;
    bool has_card = false;

    if (age <= 12) printf("You are young enough to have the discount.");
    else if (age >= 65 && has_card) printf("You are old enough to have the discount.");
    else printf("You are not eligible for the discount!");

    return 0;
}
```

Code translation: If age is 12 or less, young enough. Else (if age is larger than 12) if age is 65 or more and has a card, old enough. Else (if age is larger than 12 and smaller than 65) then not eligible.

Note that I did not use braces `{}` here, that is because when there is only 1 statement for each condition, it's not needed. Recall that compound statements (braces) are used to put multiple statements in places that expect 1 statement only.

## 4. Clap-7

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    for (int i = 1; i <= 100; ++i) {
        bool isDiv = false;
        if (!(i % 7)) {
            printf("*clap*\n");
            isDiv = true;
        }
        if (!(i % 3)) {
            printf("*click*\n");
            isDiv = true;
        }
        if (!(i % 5)) {
            printf("*stomp*\n");
            isDiv = true;
        }
        if (!isDiv) printf("%d\n", i);
    }

    return 0;
}
```

You may ask what is `!(i % 7)`. To understand this you need to know that in C, `false` is just an alias of `0`, and any other number (usually `1`) represent `true`. So when `i` is a multiple of 7, `i % 7` will evaluate as `0`, then the NOT operator will reverse the `0` to `1`, then the whole condition will evaluate as `true`.

I noticed a lot of students used `if (i % 7 != 0 && i % 5 != 0 && i % 3 != 0)`, while that works, you basically retyped the previous conditions once again, so it increases the chance of a typo and hence the chance of error. A better practice is to use a `bool` variable as shown.

`else if` won't work here, because for a number like 35, which is a multiple of 3, 5, and 7, it will see that it is a muliple of 7 first. Then, because the first condition is fulfilled, it won't check the other conditions. The result is, the console will only output `*clap*` for 35, which is not correct.

## 5. Area of circle

```c
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>

float circleArea(float r) {
    return r * r * M_PI;
}

int main() {
    float r;

    printf("Please enter the radius: ");
    scanf("%f", &r); //&r means the memory address of the variable r

    printf("Area of circle = %f * %f * %f = %f", r, r, M_PI, circleArea(r));
    return 0;
}
```

Here I used the `M_PI` macro that is defined within the `math.h` library. This macro along with several other similar macros are not part of the C standard, but most if not all compilers include them for convenience. As it is not part of the C standard, it cannot be enabled by default, so you need to `#define _USE_MATH_DEFINES` yourself, which enables the macros.

You may ask why not omit the braces `{}` when defining the function, it is because by definition, function definitions expect a compound statement, so I must use the braces here.

## 6. Fibonacci sequence

```c
#include <stdio.h>

int main() {
    int fibonacci[20] = {1, 1}; //Initialise the first 2 elements of the array as 1, others elments will become 0

    for (int i = 2; i < 20; ++i) fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];

    while (1) {
        int index;
        printf("Select an item from the array: ");
        scanf("%d", &index);
        printf("%d\n", fibonacci[index]);
    }

    return 0;
}
```

Note that if the array is not initialised, i.e. `int fibonacci[20];`, that elements in the array will be undefined, and it will give you a garbage value (basically a random number) if you try to access it. In my code, I initialised the first 2 elements as `1`, and then all other elements got implicitly initialised as `0`.

## 7. Triangle

```c
#include <stdio.h>

int main() {
    int height;
    printf("Please enter the height: ");
    scanf("%d", &height);
    printf("\n");

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < height - 1 - i; ++j) printf(" ");
        for (int j = 0; j <= 2 * i; ++j) printf("*");
        printf("\n");
    }

    return 0;
}
```

Some of you over-complicated this problem, one of you is even thinking of concatenating strings. Actually it can be easily done with some simple maths.

Suppose the first line of the triangle is line 0, which is represented by the variable `i` in the code.

Notice that the number of spaces before the first star on each line is really `height - line number - 1`, hence `for (int j = 0; j < height - 1 - i; ++j) printf(" ");`, which prints a space `height - line number - 1` times.

Then, notice that the number of stars in each line is really `1 + line nubmer * 2`, hence `for (int j = 0; j <= 2 * i; ++j) printf("*");`, which prints the `*` character `1 + line nubmer * 2` times. Note the use of `<=` instead of the usual `<` in the condition of this for loop.

## 8. Find greatest number(BONUS)

```c
#include <stdio.h>

int main() {
    int num;
    printf("Please enter the number: ");
    scanf("%d", &num);

    int max = 0;
    for (; num; num /= 10) {
        int temp = num % 10;
        if (temp < 0) temp = -temp;
        if (temp > max) max = temp;
    }

    printf("\nThe greatest number is %d.", max);

    return 0;
}
```

This code is slightly more difficult to understand than the previous ones, so I will walk it through step by step.

Suppose the input is 693.

First the right-most digit `3` is extracted by taking the remainder of dividing by 10. It is compared with the current `max`, which was initialised as `0` (the smallest possible number). Then `3` is saved into `max` as it is larger than `0`.

The first loop cycle ends here and the number is divided by 10, now the new right-most digit is `9` (recall that integer division in C truncates the decimal points). The loop runs again since `69 != 0`, and `9` is saved into `max`.

The second loop cycle ends here and then the new right-most digit `6` is not saved into `max`, since `6` is smaller than `9`.

The third loop cycle ends here and `6` (`num`) is divided by `10`, which evaluates as `0`. Now the condition `num` is `false`, and the loop exits.

Now you have the biggest digit `9` saved in `max`.

`if (temp < 0) temp = -temp;` is for in case the input value is negative.

**Very important to initialise `max` to `0`, otherwise it is meaningless to compare the digit to a garbage value.**

This method won't work for numbers larger than `INT_MAX` or smaller than `INT_MIN`.

Note: The for loop
```c
for (; num != 0; num /= 10) {
    int temp = num % 10;
    if (temp > max) max = temp;
}
```
will be expanded to
```c
{
    ; //This is an empty statement, i.e. a statement that does nothing
    while (num != 0) {
        int temp = num % 10;
        if (temp > max) max = temp;
        num /= 10;
    }
}
```
The first `;` is needed, otherwise there will be only 2 statements in the parenthesis and will cause a syntax error.

## 9. Reverse integer(BONUS)
```c
#include <stdio.h>

int ReverseInteger(int input) {
    int reversed = 0;
    for (; input; input /= 10) { //Shift the digits to the right for each cycle
        reversed *= 10; //Shift the result left by 1 digit
        reversed += input % 10;
    }

    return reversed;
}

int main() {
    int num;
    printf("Please enter the number: ");
    scanf("%d", &num);

    printf("\nThe reversed number is %d", ReverseInteger(num));

    return 0;
}
```

The same for loop trick from Q8 is used here.

The code basically shifts new digit from the previous cycle 1 digit to the left at the start of each cycle and add a new digit to it. That is all.

This method won't work for numbers larger than `INT_MAX` or smaller than `INT_MIN`.