# HKUST Robotics Team Software Tutorial 2019 - Tutorial 1 GPIO HW (Solutions)

<hr>
The following solutions are using the functions defined below. Please be reminded that these functions are self-defined and the solutions below will be using these solutions.

```C
//-----------------------------INITIALIZATION-------------------------
void init_everything(void){
	HAL_GPIO_InitPin(LED1_GPIO_Port,LED1_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	HAL_GPIO_InitPin(LED2_GPIO_Port,LED2_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	HAL_GPIO_InitPin(LED3_GPIO_Port,LED3_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
	HAL_GPIO_InitPin(BUTTON1_GPIO_Port,BUTTON1_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
	HAL_GPIO_InitPin(BUTTON2_GPIO_Port,BUTTON2_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
	HAL_GPIO_InitPin(BUTTON3_GPIO_Port,BUTTON3_Pin,GPIO_MODE_INPUT,GPIO_PULLUP);
	HAL_GPIO_InitPin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
}
//-----------------------------BUTTONS-------------------------
typedef enum{
    BUTTON1=0,
    BUTTON2,
    BUTTON3
}Buttons;

typedef struct{
	GPIO_TypeDef * port;
	uint16_t pin;
	Buttons id;
	uint8_t last_state;
	uint8_t current_state;
	uint32_t press_down,lift_up;//The rising and falling edge "ticks"
}Button_Struct;

#define btn_clicked 1
#define btn_released 0

static Button_Struct BUTTONS[3]= {
		{BUTTON1_GPIO_Port,BUTTON1_Pin,BUTTON1,0,0,0,0},
		{BUTTON2_GPIO_Port,BUTTON2_Pin,BUTTON2,0,0,0,0},
		{BUTTON3_GPIO_Port,BUTTON3_Pin,BUTTON3,0,0,0,0}
};

uint8_t button_pressed(Buttons btnid){
	return !HAL_GPIO_ReadPin(BUTTONS[btnid].port,BUTTONS[btnid].pin);
}
void button_curr_update(void){
	for(int i=0;i<3;i++){
		BUTTONS[i].current_state = button_pressed(BUTTONS[i].id);
		if(BUTTONS[i].current_state==btn_clicked && BUTTONS[i].last_state==btn_released)BUTTONS[i].press_down=HAL_GetTick();
		if(BUTTONS[i].current_state==btn_released && BUTTONS[i].last_state==btn_clicked)BUTTONS[i].lift_up = HAL_GetTick();
	}
}
uint8_t button_edgetrigger(Buttons btnid,uint8_t edge){
	switch(edge)
	{
		case btn_clicked:
		{
			if(BUTTONS[btnid].current_state==btn_clicked && BUTTONS[btnid].last_state==btn_released)return 1;
			else return 0;
		}
		case btn_released:
		{
			if(BUTTONS[btnid].current_state==btn_released && BUTTONS[btnid].last_state==btn_clicked)return 1;
			else return 0;
		}
		default:return 0;
	}
}
void button_last_update(void){
	for(int i=0;i<3;i++){
		BUTTONS[i].last_state = BUTTONS[i].current_state;
	}
}
//-----------------------------LED---------------------------
typedef enum{
	LED1,
	LED2,
	LED3
}Leds;
typedef struct{
	Leds id;
	GPIO_TypeDef *port;
	uint16_t pin;
}LedStruct;

static LedStruct LEDS[3]={
		{LED1,LED1_GPIO_Port,LED1_Pin},
		{LED2,LED2_GPIO_Port,LED2_Pin},
		{LED3,LED3_GPIO_Port,LED3_Pin}
};
void led_on(Leds ledid){
	HAL_GPIO_WritePin(LEDS[ledid].port,LEDS[ledid].pin,GPIO_PIN_SET);
}
void led_off(Leds ledid){
	HAL_GPIO_WritePin(LEDS[ledid].port,LEDS[ledid].pin,GPIO_PIN_RESET);
}
//-----------------------------BUZZER---------------------------
void buzzer_on(){
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
}
void buzzer_off(){
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}
```
### Classwork 1
```C
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();     //Initialize most GPIOs on board
  MX_DMA_Init();      //Initialize DMA

  //Self-defined function
  init_everything();
  while(1){
	  static uint32_t this_ticks = 0;
	  if(HAL_GetTick()-this_ticks>=20){

		  if(button_pressed(BUTTON1))led_on(LED1);
		  else led_off(LED1);

		  if(button_pressed(BUTTON2))led_on(LED2);
		  else led_off(LED2);

		  if(button_pressed(BUTTON3))led_on(LED3);
		  else led_off(LED3);

		  this_ticks=HAL_GetTick();
	  }
  }
}
```
### Classwork 2
```C
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();     //Initialize most GPIOs on board
  MX_DMA_Init();      //Initialize DMA

  //Self-defined function
  init_everything();

  uint8_t beep_count=2,buzzer_state=0,led_state=0,led_no=0;
  while(1){
	  static uint32_t this_ticks = 0,beep_timer=0,led_timer;
	  if(HAL_GetTick()-this_ticks>=20){

		  if(HAL_GetTick()-beep_timer>=300){
			  if(beep_count){
				  if(!buzzer_state){
					  buzzer_on();
					  buzzer_state = !buzzer_state;
				  }else{
					  buzzer_off();
					  beep_count--;
					  buzzer_state = !buzzer_state;
				  }
			  }

			  beep_timer = HAL_GetTick();
		  }

		  if(HAL_GetTick() - led_timer >=500){
			  if(!led_state){
				  switch(led_no){
				  	  case 0:led_on(LED1);break;
				  	  case 1:led_on(LED2);break;
				  	  case 2:led_on(LED3);break;
				  	  default:break;
				  }
				  led_state = !led_state;
			  }else{
				  led_off(LED1);
				  led_off(LED2);
				  led_off(LED3);

				  led_state = !led_state;
				  led_no++;
				  led_no %= 3;
			  }
			  led_timer = HAL_GetTick();
		  }
		  this_ticks=HAL_GetTick();
	  }
  }
}
```
### Classwork 3.1 + 3.2
```C
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();     //Initialize most GPIOs on board
  MX_DMA_Init();      //Initialize DMA

  //Self-defined function
  init_everything();

  static uint8_t btn_pressed = 0,next_btn=0;
  while(1){
	  static uint32_t this_ticks = 0;

	  if(HAL_GetTick()-this_ticks>=5){

		  button_curr_update();

		  if(btn_pressed==0){
			  if(button_pressed(BUTTON1))btn_pressed=1;
			  else if(button_pressed(BUTTON2))btn_pressed=2;
			  else if(button_pressed(BUTTON3))btn_pressed=3;
		  }else{
			  if(next_btn==0){	//Update the next btn
				  if(btn_pressed!=1 && button_pressed(BUTTON1))next_btn=1;
				  else if(btn_pressed!=2 && button_pressed(BUTTON2))next_btn=2;
				  else if(btn_pressed!=3 && button_pressed(BUTTON3))next_btn=3;
			  }
		  }
		  if(btn_pressed!=0){	//If a button operation currently being executed
			  if(button_edgetrigger(btn_pressed-1,btn_released)){
				  btn_pressed=next_btn;
				  next_btn = 0;
			  }
		  }

		  //Update the leds
		  switch(btn_pressed){	// 4 button states
		  	  case 0:led_off(LED1);led_off(LED2);led_off(LED3);break;	//All led off, no buttons being pressed
		  	  case 1:led_on(LED1);led_off(LED2);led_off(LED3);break;	//Only led1 on, button1 being pressed
		  	  case 2:led_on(LED2);led_off(LED3);led_off(LED1);break;	//Only led2 on, button2 being pressed
		  	  case 3:led_on(LED3);led_on(LED1);led_on(LED2);break;		//All led on, button3 being pressed
		  	  default:break;
		  }

		  this_ticks=HAL_GetTick();
		  button_last_update();
	  }
  }
}
```
### Classwork 3.3 + 3.4
```C
typedef enum{
	NORMAL=0,
	FLASHING
}Mode;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();     //Initialize most GPIOs on board
  MX_DMA_Init();      //Initialize DMA

  init_everything();//Self-defined function

  static Mode curLedMode[3] = {NORMAL};
  static uint8_t led_state[3]={0};//This stores the state of the led
  static uint8_t btn_click_state[3]={0}//This stores "1" once 1 button click has been found and is refreshed to 0 every 300ms

  while(1){
	  static uint32_t this_ticks = 0,flash_ticks[3]={0},double_clicks[3]={0};

	  if(HAL_GetTick()-this_ticks>=10){
		  button_curr_update(); //Self-defined function

		  for(int i=0;i<3;i++){//Double Click Checker
			  if(button_edgetrigger(BUTTONS[i].id,btn_clicked) && !btn_click_state[i]){	//1st click of the double click
				  double_clicks[i]=HAL_GetTick();
				  btn_click_state[i]=1;
			  }else if(button_edgetrigger(BUTTONS[i].id,btn_clicked) && btn_click_state[i]){//2nd click of the double click
				  if(HAL_GetTick()-double_clicks[i]<=300){//Double click within 300ms
					  if(i==2){
						  for(int j=0;j<3;j++)curLedMode[j] = !curLedMode[j];
					  }else{
						  curLedMode[i]=!curLedMode[i];
					  }
				  }
				  btn_click_state[i]=0;
			  }
	  	  }



		  //Change the state of the LED depending on the Button clicks
		  for(int i=0;i<3;i++){
			  switch(curLedMode[i]){
			  	  case NORMAL:
			  	  {
			  		  if(i==2){	//If Button3 is in normal
			  			  if(button_edgetrigger(i,btn_clicked)){//Toggle button 3
			  				  led_state[0] = !led_state[0];
			  				  led_state[1] = !led_state[1];
			  				  led_state[2] = !led_state[2];
			  			  }
			  		  }else{
				  		  if(button_edgetrigger(i,btn_clicked)){//Toggle button 1/2
				  			  led_state[i] = !led_state[i];	//Toggle the led
				  		  }
			  		  }
			  		  break;
			  	  }
			  	  case FLASHING:
			  	  {
			  		  if(HAL_GetTick()-flash_ticks[i]>=300){
			  			led_state[i]=!led_state[i];
			  			flash_ticks[i]=HAL_GetTick();
			  		  }
			  		  break;
			  	  }
			  	  default:break;
			  }
		  }

		  for(int i=0;i<3;i++){//Led Control
			  if(led_state[i]==1){
				  led_on(i);
			  }else{
				  led_off(i);
			  }
		  }

		  button_last_update();
		  this_ticks=HAL_GetTick();
	  }

  }
}
```

### Classwork 3.5
```C
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();     //Initialize most GPIOs on board
  MX_DMA_Init();      //Initialize DMA
  
  init_everything();//Self-defined function

  static uint8_t event_triggered = 0,beep_count=0,buzzer_state=0;
  while(1){
	  static uint32_t this_ticks = 0,beep_ticks=0;
	  if(HAL_GetTick()-this_ticks>=10){
		  button_curr_update();

		  if(event_triggered){//If currently beeping
			  if(HAL_GetTick()-beep_ticks>=300){//Runs every 300ms
				  if(beep_count>0){
					  if(buzzer_state){//Buzzer currently on
						  buzzer_off();
						  beep_count--;
						  buzzer_state=0;
					  }else{
						  buzzer_on();
						  buzzer_state=1;
					  }
				  }else{
					  event_triggered=0;
				  }
				  beep_ticks=HAL_GetTick();
			  }
		  }else{//If currently NOT beeping
			  if(button_edgetrigger(BUTTONS[0].id,btn_clicked)){
				  event_triggered=1;
				  beep_count=1;
			  }else if(button_edgetrigger(BUTTONS[1].id,btn_clicked)){
				  event_triggered=1;
				  beep_count=2;
			  }else if(button_edgetrigger(BUTTONS[2].id,btn_clicked)){
				  event_triggered=1;
				  beep_count=3;
			  }
		  }

		  button_last_update();
		  this_ticks=HAL_GetTick();
	  }

  }
}
```
