/***********************************************************
 *
 *  Exemplo de criação de task
 *
 ***********************************************************/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define LED1 19
#define LED2 16
#define BTN 12

/* Handles */
TaskHandle_t xTask1 = NULL;
TaskHandle_t xTimer1, xTimer2;

/* Protótipo das tasks */
void vTask1(void *pvParameters);

void callBackTimer1(TimerHandle_t xTimer);
void callBackTimer2(TimerHandle_t xTimer);

void setup()
{
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  xTimer1 = xTimerCreate("TIMER1", pdMS_TO_TICKS(1000), pdTRUE, 0, callBackTimer1);
  xTimer2 = xTimerCreate("TIMER2", pdMS_TO_TICKS(10000), pdFALSE, 0, callBackTimer2);
  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1);
  xTimerStart(xTimer1, 0);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void vTask1(void *pvParameter)
{
  uint8_t debouncingTime = 0;
  while (1)
  {
    if ((digitalRead(BTN) == LOW) && xTimerIsTimerActive(xTimer2) == pdFALSE)
    {
      debouncingTime++;
      if (debouncingTime >= 10)
      {
        debouncingTime = 0;
        digitalWrite(LED2, HIGH);
        xTimerStart(xTimer2, 0);
        xTimerStop(xTimer1, 0);
        Serial.println("Timer 2 start.");
      }
    }
    else
      debouncingTime = 0;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void callBackTimer1(TimerHandle_t xTimer)
{
  Serial.println("callBT1");
  digitalWrite(LED1, !digitalRead(LED1));
}

void callBackTimer2(TimerHandle_t xTimer)
{
  Serial.println("callBT2");
  digitalWrite(LED2, LOW);
  xTimerStart(xTimer1, 0);
}