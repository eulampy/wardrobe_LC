#define LED_BUILTIN 1
#define LED_STRIP 0

// граничное значение для датчика
#define SENS_TRSH_VAL	(500)	
// 0....................290...310......v.......600...
// не_подключен          открыто       x       6 см
// (x > 100) && (x < SENS_TRSH_VAL) - открыто
// (x >= SENS_TRSH_VAL) - закрыто

#define ON_TIMEOUT	(1200)	// таймаут включения (в интервалах по 500 мс)

// the setup function runs once when you press reset or power the board
void setup() {
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	
	pinMode(LED_STRIP, OUTPUT);
	digitalWrite(LED_STRIP, LOW);
}

uint16_t P2_ADC1_value = 0; // внешний датчик
uint16_t P4_ADC2_value = 0; // внутренний датчик
uint16_t ext_sensor_value = 0;
uint16_t int_sensor_value = 0;
uint8_t samples_counter = 0;
uint16_t on_counter = 0;
uint8_t led_toggle = 0;
uint8_t doors_open = 0;

enum { OFF, ON, WAIT_OFF} state = OFF;


// the loop function runs over and over again forever
void loop() {
	if (samples_counter < 8) {
		P2_ADC1_value += analogRead(1); //Read P2 внешний датчик
		P4_ADC2_value += analogRead(2); //Read P4 внутренний датчик
		samples_counter++;
	}else{
		ext_sensor_value = P2_ADC1_value / 8; 
		int_sensor_value = P4_ADC2_value / 8; 

		//if( (int_sensor_value < SENS_TRSH_VAL) || ((ext_sensor_value > 100) && (ext_sensor_value < SENS_TRSH_VAL)) ) // если хотя бы один датчик в положении "открыто"
		if( (int_sensor_value < SENS_TRSH_VAL) || (ext_sensor_value < SENS_TRSH_VAL) ) // если хотя бы один датчик в положении "открыто"
			doors_open = 1;
		else
			doors_open = 0;
	
		switch(state){
			case OFF:
				if(doors_open){
					state = ON;
					digitalWrite(LED_BUILTIN, HIGH);
					digitalWrite(LED_STRIP, HIGH);
				}
				break;
			case ON:
				if(doors_open){
					if(on_counter < ON_TIMEOUT){
						on_counter++;
					}else{	// двери открыты слишком долго
						on_counter = 0;
						digitalWrite(LED_STRIP, LOW);
						state = WAIT_OFF;
					}
				}else{
					on_counter = 0;
					digitalWrite(LED_BUILTIN, LOW);
					digitalWrite(LED_STRIP, LOW);
					state = OFF;
				}
				break;
			case WAIT_OFF:
				if(led_toggle){
					led_toggle = 0;
					digitalWrite(LED_BUILTIN, LOW);
				}else{
					led_toggle = 1;
					digitalWrite(LED_BUILTIN, HIGH);
				}
				if(!doors_open)
				{
					digitalWrite(LED_BUILTIN, LOW);
					led_toggle = 0;
					state = OFF;
				}
				break;

		} // end of switch

		samples_counter = 0;
		P2_ADC1_value = 0;
		P4_ADC2_value = 0;
	}

	delay(52);	// comment
}
