#### Author: Ho Yu Yat (yyhoai@connect.ust.hk) (2019)

##### A and B Alternative Solutions: Caner Demirer (cdemirer@connect.ust.hk) (2019)

# Homework 0 Answers

If you have any questions about the answers, you are very welcome to WhatsApp me at 68499100 or by email.

These are by no means THE best way to accomplish the task, it's the best I can do.

## Simple Calculator
```c
#include <stdio.h>

int main() {
    char again;

    do {
        char operator;
        int num1, num2, result;

        printf("Select a mode of operation (+, -, *, /, %%): ");
        scanf(" %c", &operator);
        printf("First number: ");
        scanf("%d", &num1);
        printf("Second number: ");
        scanf("%d", &num2);

        switch (operator) {
            case '+': {
                result = num1 + num2;
                break;
            }
            case '-': {
                result = num1 - num2;
                break;
            }
            case '*': {
                result = num1 * num2;
                break;
            }
            case '/': {
                result = num1 / num2;
                break;
            }
            case '%': {
                result = num1 % num2;
                break;
            }
            default: {
                break;
            }
        }

        printf("%d %c %d = %d\n", num1, operator, num2, result);
        printf("Again? ");
        scanf(" %c", &again);
    } while (again == 'y');

    return 0;
}
```

Very self-explanatory. First get the inputs, then do the caclulations, finally display the result.

## Mesmerizing Sin Curve
```c
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>

int main() {
    const int periods = 3;
    const int x_size = 20;
    const int y_size = 15;
    const int amplitude = x_size / 2;

    for (int i = 0; i < y_size * periods; ++i) {
        for (int j = 0; j < amplitude + sin(2 * M_PI * i / y_size) * amplitude; ++j) printf(" ");
        printf("*\n");
    }

    return 0;
}
```

The sin curve will occupy a total of `y_size * periods` lines, hence `for (int i = 0; i < y_size * periods; ++i)`.

With `i` as the line number, the number of spaces before the `*` of each line will be `amplitude + sin(2 * M_PI * i / y_size) * amplitude`, where `amplitude` = `x_size / 2`.

Finally display the star and print a newline.

## A and B

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    char ab[] = {'a', 'b', 'b', 'a', 'a', 'a', 'a', 'b', 'b', 'b'};
    int len = sizeof(ab) / sizeof(char); //The number of elements in the array

    while (true) {
        bool done = true;

        for (int i = 0; i < len - 1;) {
            if ((ab[i] == 'a' && ab[i + 1] == 'b') || (ab[i] == 'b' && ab[i + 1] == 'a')) {
                for (int j = i + 2; j < len; ++j) ab[j - 2] = ab[j]; //Shift all elements to the left twice
                len -= 2; //Store the new length
                done = false;
            } else ++i;
        }

        if (done) break;
    }

    for (int i = 0; i < len; ++i) printf("%c", ab[i]);

    return 0;
}
```

The whole idea is straight-forward, basically whenever the situation `'a', 'b'` or `'b', 'a'` occurs, shift elements to the left twice starting from `i + 2`. But there are a few details that needs attention in this code.

1. The condition of the for-loop for looping through the elements in the char array must be `i < len - 1`. This is because the (i + 1)th element will be accessed, so the loop can only proceed until the penultimate element without accessing outside the array.

2. Only `++i` if no elements are needed to be removed. This is because after removing 2 elements, the new characters to be checked will be moved to the current index. You only need to `++i` if the current element is to be kept.

3. Remember to `len -= 2` to prevent checking elements that became meaningless after removing some characters.

The reason for putting the whole for-loop into another while-loop is because new `'a', 'b'` pairs might occur after looping through once, so it needs to repeatedly remove these new pairs until none is left.

### A and B - 2 Alternative Solutions

#### 1) Using the same idea

This one below uses C standard library functions to implement an idea (similar to the one above) more easily.
Instead of starting our search for `ab`s or `ba`s from the beginning each time, we do a single sweep on the array (not counting the shifting of elements). Whenever we modify some part (suffixes of the array in this case) `[j, ∞)` of the array, we only need to look at `[j-1, ∞)` for the rest of our search. There is an edge case to handle (`j = 0`).

```c
#include <stdio.h>
#include <string.h>

char arr[] = {'a', 'b', 'b', 'b', 'a'}; // Can be any other valid input

int main(int argc, char **argv) {

    int len = sizeof(arr) / sizeof(arr[0]);
    
    
    // ##### O(N^2) #####
    for (int i = 0; i < len-1; i++) {
        if (arr[i] + arr[i+1] == 195) {
            memmove(arr+i, arr+i+2, (len-i-2) * sizeof(char));
            len -= 2;
            i = (i > 0 ? i-2 : -1);
        }
    }
    for (int i = 0; i < len; i++) printf("%c ", arr[i]); printf("\n");
    
    
    return 0;
}
```

Note that `'a' + 'b' == 195`. [ASCII Table](http://www.asciitable.com/)


#### 2) Using a different point of view

The solution below makes use of the observation that the order of the elements in the input array does not affect the output. You can think of the As and Bs as matter and anti-matter, and visualize the events that follow.

This one is more efficient than the 2 other solutions above. Its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is better.

```c
#include <stdio.h>

char arr[] = {'a', 'b', 'b', 'b', 'a'}; // Can be any other valid input

int main(int argc, char **argv) {
    
    int len = sizeof(arr) / sizeof(arr[0]);
    
    
    // ##### O(N) #####
    int num = 0;
    for (int i = 0; i < len; i++) num += (arr[i] == 'a' ? 1 : -1);
    for (int i = 0; i < num; i++) printf("a ");
    for (int i = 0; i > num; i--) printf("b ");
    printf("\n");
    
    
    return 0;
}
```

## Number of inner parameter (BONUS)
```c
#include "count.h"

int CountInnerParameter(char in[], int len) {
    int depth = 0, count = 0;
    for (int i = 0; i < len; ++i) {
        if (in[i] == '(') {
            if (depth) ++count;
            ++depth;
        } else if (in[i] == ')') --depth;
    }

    return count;
}
```

The variable `depth` records how many layers of brackets are the loop currently in. e.g. For the brackets `((()))`, after the 3rd `(`, the depth will be `3`.

The code is simple. Whenever a `(` is seen, increment `depth`. If `depth` is not `0` before incrementing, then it is an inner parameter, therefore, increment `count`.

Then, whenever a `)` is seen, decrement `depth`.
