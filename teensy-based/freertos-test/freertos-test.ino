/******************************************************************************
FreeRTOS timed blink example

Byron Jacquot @ SparkFun Electronics>

Porting Marshall Taylor's out-of-synch blinker example
to an RTOS, to show off basic features.

Resources:
Requires Bill Greiman's port of Free RTOS to Arduino
https://github.com/greiman/FreeRTOS-Arduino

Development environment specifics:
Arduino 1.6.5
SparkFun RebBoard with additional LED on pin 12.

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Distributed as-is; no warranty is given.
******************************************************************************/

#include <FreeRTOS_ARM.h> 

// The LED is attached to pin 13 on Arduino.
// Plus another LED on 12
const uint8_t LED1_PIN = 13;
const uint8_t LED2_PIN = 2;

/*
 * First blinker does 200 msec on, 200 msec off.
 */
static void ThreadLED1(void* arg) 
{
  pinMode(LED1_PIN, OUTPUT);
  
  while (1) 
  {
    vTaskDelay(200);
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(200);
    digitalWrite(LED1_PIN, LOW);
  }
}

/*
 * Second blinker does 210 msec on, 210 msec off.
 */
static void ThreadLED2(void* arg) 
{
  pinMode(LED2_PIN, OUTPUT);
  
  while (1) 
  {
    vTaskDelay(210);
    digitalWrite(LED2_PIN, HIGH);
    vTaskDelay(210);
    digitalWrite(LED2_PIN, LOW);
  }
}


void setup() 
{
  portBASE_TYPE status1, status2;

  Serial.begin(9600);
  
  // create two parallel LED tasks at priority two
  status1 = xTaskCreate(ThreadLED1, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  status2 = xTaskCreate(ThreadLED2, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);

  if ( (status1 != pdPASS ) || (status2 != pdPASS)) {
    Serial.println(F("Creation problem"));
    while(1);
  }
  // start scheduler - should never return
  vTaskStartScheduler();
  Serial.println(F("Scheduler Returned"));
  while(1);
}

//------------------------------------------------------------------------------
// WARNING idle loop has a very small stack (configMINIMAL_STACK_SIZE)
// loop must never block
void loop() 
{
}

