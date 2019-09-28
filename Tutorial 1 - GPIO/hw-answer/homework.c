#include "main.h"

//Design ideologies:
// More small functions >>> Few giant functions
// Reusability and Extendability is most important (Dont-Repeat-Yourself)
// then Readability (comments are good, self documenting code is best)
// No "magic numbers", use defines, constants, variables etc
// ALWAYS INDENT CORRECTLY
// No blocking (long while loops, HAL_Delay)

//*************************************************************************************
//Hardware access stuff
//Enums are always good
typedef enum {
	BUTTON1=0,
	BUTTON2,
	BUTTON3,
	//This trick means the NumButtons will equal how many buttons there are (how many enum values)
	//This works as long as the enum values are simple (count up from 0)
	NumButtons,
} Button;

static GPIO_TypeDef *const BUTTON_PORTS[NumButtons] = {
	BUTTON1_GPIO_Port,
	BUTTON2_GPIO_Port,
	BUTTON3_GPIO_Port,
};
static const uint16_t BUTTON_PINS[NumButtons] = {
	BUTTON1_Pin,
	BUTTON2_Pin,
	BUTTON3_Pin,
};

uint8_t read_button(Button b) {
	return !HAL_GPIO_ReadPin(BUTTON_PORTS[b], BUTTON_PINS[b]);
}

void buzzer_write(uint8_t val) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, val);
}

typedef enum {
	LED1=0,
	LED2,
	LED3,

	NumLEDs,
} LED;
static GPIO_TypeDef *const LED_PORTS[NumLEDs] = {
	LED1_GPIO_Port,
	LED2_GPIO_Port,
	LED3_GPIO_Port,
};
static const uint16_t LED_PINS[NumLEDs] = {
	LED1_Pin,
	LED2_Pin,
	LED3_Pin,
};
void write_led(LED led, uint8_t val) {
	HAL_GPIO_WritePin(LED_PORTS[led], LED_PINS[led], val);
}

//*************************************************************************************
//Separate systems for controlling LEDs, Buzzer and Buttons
//Each of them have update functions that run separate from the main code
//This makes the main code mostly not have to deal with timing and state
//This section is pretty reusable for alot of tasks

//use this variable which is only set at the beginning of the loop instead of HAL_GetTick() directly
//to avoid the rare case where GetTick() changes in the middle of a loop iteration
static uint32_t this_ticks = 0;
uint8_t every_ms(uint32_t n) {
	static uint32_t last_ticks = 0;
	this_ticks = HAL_GetTick();
	if ((this_ticks - last_ticks) < n) return 1;
	last_ticks = this_ticks;

	return 0;
}

//TODO: LED and Buzzer flashing/beeping (queue actions for the future)

//For leds just use an array to store the state and write the GPIO at the end of the loop iter
static uint8_t led[NumLEDs] = {0, 0, 0};
void update_leds(void) {
	for(int i=0; i<NumLEDs; i++) {
		write_led(i, led[i]);
	}
}

//For buzzers use variables for start time and end time and control the buzzer separate from queuing
//so that you can make a beep last certain amount of time
//separately from the main code
#define TIME_NEVER 0xFFFFFFFF //max of uint32_t
static uint32_t buzzer_start_time = TIME_NEVER;
static uint32_t buzzer_end_time = TIME_NEVER;
static uint32_t buzzer_restart_time = TIME_NEVER;
static uint32_t buzzer_beep_count = 0;
void buzzer_off() {
	buzzer_end_time = buzzer_start_time = buzzer_restart_time = TIME_NEVER;
}
void buzzer_beep_multi(uint32_t on, uint32_t off, uint32_t times) {
	if (times == 0) buzzer_off();
	else {
		buzzer_beep_count = times;
		buzzer_start_time = this_ticks;
		buzzer_end_time = buzzer_start_time + on;
		buzzer_restart_time = buzzer_end_time + off;	
	}
}
void buzzer_on() { //on
	buzzer_start_time = 0;
	buzzer_end_time = buzzer_restart_time = TIME_NEVER; //never end
}
void buzzer_beep(uint32_t ticks) { //beep from now till 'ticks' ms later
	buzzer_beep_multi(ticks, 0, 1);
}
//write the GPIO
void update_buzzer(void) {
	if (this_ticks < buzzer_start_time) {
		buzzer_write(0);
	} else if (this_ticks < buzzer_end_time) {
		buzzer_write(1);
	} else if (this_ticks < buzzer_restart_time) {
		buzzer_write(0);
	} else {
		buzzer_write(buzzer_beep_count > 1);
		buzzer_beep_multi( //Kinda recursive, so I dont repeat the logic
			buzzer_end_time - buzzer_start_time, //I dont save on and off time but I can recover it
			buzzer_restart_time - buzzer_end_time, 
			buzzer_beep_count-1); //decrement
	}
}

//The button update maintains a history of previous button events that can be used to detect multiclicks and holding
//The key to extensibility is to have a very layered architecture with a lot of reusable functions
//Another good way to do this is to design a finite state machine that changes based on button state and time to detect multiclicks and holding
// - This would be more efficient but less easily extendable 
typedef enum {
	BTN_NONE,
	BTN_CLICKED,
	BTN_RELEASED,
} ButtonEventType;
typedef struct {
	ButtonEventType evt;
	uint32_t ticks;
} ButtonEvent;
#define BUTTON_HISTORY_LENGTH 6
static ButtonEvent btn_history[NumButtons][BUTTON_HISTORY_LENGTH]; //0 is the latest event, increasing goes back in time
void push_button_event(Button b, ButtonEventType evt) {
	//Countdown from the last possible index to 1
	for (int i=BUTTON_HISTORY_LENGTH-1; i>0; i--) {
		//Copy the event 'backwards'
		//[1] goes to [2]
		//[0] goes to [1]
		//the last index is overwritten and the first index is now free to be replaced
		//it is very important that this be done in a countDOWN for loop, or else
		//[0] goes to [1] which then goes to [2] etc etc, 
		btn_history[b][i] = btn_history[b][i-1];

		//For extra credit: the better way of doing this is to use a Circular Buffer
		//In this we succesively store the latest event in a ever increasing index (wrapping around)
		//we keep track of this index so we can retrieve the latest index
		//This means we would not have to copy the events every time a new one comes in
		//however its slightly more confusing and this works fines for small histories
	}
	btn_history[b][0].evt = evt;
	btn_history[b][0].ticks = this_ticks;
}
//Return the nth previous button event (0 is most recent)
ButtonEvent btn_event_n(Button b, uint8_t n) {
	return btn_history[b][n];
}
void update_btns(void) {
	static uint8_t prev_pressed[NumButtons] = {0};
	for (int i=0; i<NumButtons; i++) {
		uint8_t now_pressed = read_button((Button)i);
		if (now_pressed && !prev_pressed[i]) { 
			push_button_event(i, BTN_CLICKED);
		} else if (!now_pressed && prev_pressed[i]) {
			push_button_event(i, BTN_RELEASED);
		}
		prev_pressed[i] = now_pressed;
	}
}
#define NOT_THAT_EVENT 0xFFFFFFFF //max value that u32 can store
//Check if the nth-ago event was of some type and return how long ago did it occur
uint32_t btn_event_time(Button b, uint8_t n, ButtonEventType evt) {
	if (btn_event_n(b, n).evt == evt) 
		return this_ticks - btn_event_n(b, n).ticks;
	else 
		return NOT_THAT_EVENT;
}
uint32_t btn_pressed_time_n(Button b, uint8_t n) {
	return btn_event_time(b, n, BTN_CLICKED);
}
uint32_t btn_pressed_time(Button b) {
	return btn_pressed_time_n(b, 0);
}
uint8_t btn_clicked(Button b) {
	return (btn_pressed_time(b) == 0);
}
uint8_t btn_double_clicked(Button b, uint32_t threshold) {
	//double clicked == clicked now and
	// 2 events ago should be BTN_CLICKED && less than 'threshold' ticks ago
	return btn_clicked(b) && btn_event_time(b, 2, BTN_CLICKED) <= threshold;
}
uint8_t btn_triple_clicked(Button b, uint32_t threshold) {
	return btn_double_clicked(b, threshold) && btn_event_time(b, 4, BTN_CLICKED) <= threshold;
}
uint8_t btn_held_for(Button b, uint32_t threshold) {
	uint32_t time = btn_pressed_time(b);
	return (time != NOT_THAT_EVENT) && (time >= threshold);
}
uint32_t btn_released_time(Button b) {
	return btn_event_time(b, 0, BTN_RELEASED);
}
uint8_t btn_released(Button b) {
	return (btn_released_time(b) == 0);
}

//*************************************************************************************
//Main homework code
// Uses alot of nested switch case FSMs 
// stores the number directly and converts it to binary bits for the LED
// the alternate way is to store the LED states (bits) and implement increment, rotate and lshift on the bits directly
// - i think this is simpler in some ways (rotate, maybe shift) but more complex in most ways (increment, special mode)

enum {
	MODE_ONGOING = 0,
	MODE_FINISHED,
};
static uint8_t stored_value = 0;

//get the value of the nth bit of a number
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
void display_number(uint32_t num) {
	for (int i=0; i<NumLEDs; i++) {
		led[i] = num_get_bit(num, i);
	}
}
uint32_t binary_rotation(uint32_t num, uint8_t bits) {
	uint32_t res = 0;
	for (int i=0; i<bits; i++) {
		//get the opposite bit from number (bits-1-i)
		uint32_t bit = num_get_bit(num, bits-1-i);
		//shift it into the correct place and add it into result
		res += bit << i;
	}
	return res;
}
void normal_mode(void) {
	static const uint32_t max_value = 1<<NumLEDs; //it turns out 1<<n yields the same result as 2 to the power of 'n'
	if (btn_clicked(BUTTON1)) {
		stored_value++;
		//this makes sure 'overflow' wraps around at the correct value
		//assuming stored_value only increments by one at a time
		if (stored_value == max_value) stored_value = 0;
	}
	if (btn_clicked(BUTTON2)) {
		stored_value = binary_rotation(stored_value, NumLEDs);
	}
	if (btn_clicked(BUTTON3)) {
		stored_value <<= 1; //left shift by 1
		stored_value %= max_value; //this is another way to create wrapping overflow at a specific threshold
	}
	if (btn_held_for(BUTTON1, 2000) && btn_held_for(BUTTON2, 2000)) {
		stored_value = 0;
	}
	display_number(stored_value);
}

#define SPECIAL_MODE_STAGE_TIME 500
//returns whether special mode is finished
uint8_t special_mode(void) {
	static uint8_t special_state = MODE_FINISHED;
	static uint8_t stage_counter = 0;
	static uint32_t stage_start_time = 0;
	if (special_state == MODE_FINISHED) { 
		//if special_mode() is called when special_state==MODE_FINISHED assume its because 
		//we are just entering the special_mode() state
		special_state = MODE_ONGOING;
		stage_counter = stored_value;
		stage_start_time = this_ticks;
	}

	static uint8_t stage = 0;
	if ((this_ticks - stage_start_time) >= SPECIAL_MODE_STAGE_TIME) {
		stage = !stage;
		stage_start_time = this_ticks;
		if (stage == 0) {
			// Keep silent for 500ms, meanwhile display the 3-bit value with the LEDs.
			buzzer_off();
			display_number(stored_value);
			stage_counter--;
		} else {
			// Beep with the buzzer for 500ms, turn off all LEDs.
			buzzer_on();
			display_number(0);
		}
	}

	if (stage_counter == 0) special_state = MODE_FINISHED;
	return special_state;
}

//BONUS MODE:
#define MAX_PASSWORD_LENGTH 8
Button password[MAX_PASSWORD_LENGTH] = {BUTTON1, BUTTON2, BUTTON3, 0};
uint8_t password_length = 3;
Button entered_password[MAX_PASSWORD_LENGTH] = {0};

void enter_bonus_mode(void) {
	for (int i=0; i<MAX_PASSWORD_LENGTH; i++) {
		entered_password[i] = -1;
	}
}
#define HINT_SEQUENCE_STAGE_TIME 500
void display_hint_sequence(void) {
	static uint32_t stage_start_time = 0;
	static uint32_t stage = 0;

	if ((this_ticks - stage_start_time) <= HINT_SEQUENCE_STAGE_TIME) {
		stage %= password_length+1;
		if (stage < password_length) {
			for(int i=0; i<NumLEDs; i++) {
				led[i] = (i == password[stage]); //Hint password
			}
		} else {
			for(int i=0; i<NumLEDs; i++) {
				led[i] = 1; //Flash all at the end
			}
		}
	} else if ((this_ticks - stage_start_time) <= 2*HINT_SEQUENCE_STAGE_TIME) {
		for(int i=0; i<NumLEDs; i++) {
			led[i] = 0;
		}
	} else {
		stage_start_time = this_ticks;
		stage++;
	}
}
void enter_password(void) {
	for (int i=0; i<NumButtons; i++) {
		if (btn_clicked(i)) {
			//shift back the entered password sequence and append the newest button to the end
			for (int i=0; i<(password_length-1); i++) {
				entered_password[i] = entered_password[i+1];
			}
			entered_password[password_length-1] = i;
		}
	}
}
uint8_t check_password(void) {
	for (int i=0; i<password_length; i++) {
		if (entered_password[i] != password[i]) return MODE_ONGOING;
	}
	return MODE_FINISHED;
}
uint8_t change_password(void) {
	for (int i=0; i<NumButtons; i++) {
		if (btn_released(i)) password[password_length++] = i;
		//use released so that if user holds all 3 buttons to finish password set, it does not register those as part of the password
	}
	if (password_length != 0 && (password_length == MAX_PASSWORD_LENGTH || 
		(btn_held_for(BUTTON1, 2000) && btn_held_for(BUTTON2, 2000) && btn_held_for(BUTTON3, 2000)))) {
		return MODE_FINISHED; //finished
	} else {
		return MODE_ONGOING;
	}
}
//whether to return to normal mode
uint8_t bonus_mode(void) {
	// This is an incredibly useful way to implement FSMs 
	static enum {
		HINT_SEQUENCE, ENTER_CHANGE_PASSWORD, CHANGE_PASSWORD, EXIT_CHANGE_PASSWORD
	} state = HINT_SEQUENCE;
	switch (state) {
		case EXIT_CHANGE_PASSWORD:
			if (btn_held_for(BUTTON1, 1) || btn_held_for(BUTTON2, 1) || btn_held_for(BUTTON3, 1)) {
				break;
			}
			state = HINT_SEQUENCE;
		case HINT_SEQUENCE:
			if (btn_held_for(BUTTON1, 2000) && btn_held_for(BUTTON2, 2000) && btn_held_for(BUTTON3, 2000)) {
				state = ENTER_CHANGE_PASSWORD;
				buzzer_beep(500);
				//Exploiting fall-through of switch-statements to run the change_password state immediately
			} else {
				display_hint_sequence();
				enter_password();
				return check_password();	
			}

		case ENTER_CHANGE_PASSWORD: //This is one way to run code only at the beginning of a state
			if (btn_held_for(BUTTON1, 1) || btn_held_for(BUTTON2, 1) || btn_held_for(BUTTON3, 1)) {
				break;
			}
			password_length = 0;
			state = CHANGE_PASSWORD;
			//again fall-through
		case CHANGE_PASSWORD:
			if (change_password() == MODE_FINISHED) {
				state = EXIT_CHANGE_PASSWORD;
			}
		break;
	}
	return MODE_ONGOING;
}

void gpio_homework(void) {
	// This is an incredibly useful way to implement FSMs 
	static enum {
		NORMAL_MODE, SPECIAL_MODE, BONUS_MODE
	} state = NORMAL_MODE;
	switch (state) {
		case NORMAL_MODE: 
			if (btn_triple_clicked(BUTTON2, 1000)) {
				//we intercept the triple click here and dont call normal_mode() making sure stored_value doesnt get messed up
				state = SPECIAL_MODE;
			} else if (btn_held_for(BUTTON1, 2000) && btn_held_for(BUTTON3, 2000)) {
				state = BONUS_MODE;
				enter_bonus_mode();
			} else {
				normal_mode();
			};
		break;

		case SPECIAL_MODE:
			if (special_mode() == MODE_FINISHED) 
				state = NORMAL_MODE;
		break;

		case BONUS_MODE:
			if (bonus_mode() == MODE_FINISHED) 
				state = NORMAL_MODE;
		break;
	}
}

//call within while(1) loop with nothing else
//initialize gpios outside
void main_loop(void) {
	if (every_ms(10)) return;
	//The rest only runs every 10ms

	update_btns();

	gpio_homework();

	update_leds();
	update_buzzer();
}