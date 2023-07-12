/***********************************************************
 *
 *  Exemplo de criação de task
 *
 ***********************************************************/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#define LED 19
#define BTN 12
#define TASK1_FLAG  (1<<0) // 0001
#define TASK2_FLAG  (1<<1) // 0010

/* Handles */
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;
TimerHandle_t xTimer = NULL;
EventGroupHandle_t xEventos = NULL;

static bool btnState = false;

/* Protótipo das tasks */
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void callBackTimer1(TimerHandle_t pxTimer);

int tempo = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  xEventos = xEventGroupCreate();
  xTimer = xTimerCreate("TIMER", pdMS_TO_TICKS(1000), pdTRUE, 0, callBackTimer1);
  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1Handle);
  xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask2Handle);
  xTimerStart(xTimer, 0);
}

void loop()
{
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(pdMS_TO_TICKS(500));
}

void vTask1(void *pvParameters)
{
  EventBits_t xBits;
  while(1)
  {
    xBits = xEventGroupWaitBits(xEventos, TASK1_FLAG, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("Task 1 saiu do bloqueio." + String(xBits));
  }
}

void vTask2(void *pvParameters)
{
  EventBits_t xBits;
  while(1)
  {
    xBits = xEventGroupWaitBits(xEventos, TASK2_FLAG, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("Task 2 saiu do bloqueio." + String(xBits));
  }
}

void callBackTimer1(TimerHandle_t pxTimer)
{
  tempo++;
  if (tempo == 5)
  {
    xEventGroupSetBits(xEventos, TASK1_FLAG);
  }
  else if (tempo == 10)
  {
    xEventGroupSetBits(xEventos, TASK2_FLAG);
  }
  else if (tempo == 15)
  {
    xEventGroupSetBits(xEventos, TASK1_FLAG | TASK2_FLAG);
    tempo = 0;
  }
}