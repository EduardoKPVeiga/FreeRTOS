/***********************************************************
 *
 *  Exemplo de criação de task
 *
 ***********************************************************/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LED 19
#define BTN 12

/* Handles */
QueueHandle_t xFila;

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;

static bool btnState = false;

/* Protótipo das tasks */
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void trataISR_BT()
{
  btnState = !btnState;
  if (btnState)
  {
    vTaskSuspend(task1Handle);
  }
  else
  {
    vTaskResume(task1Handle);
  }
}

void setup()
{
  BaseType_t xReturned;

  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), trataISR_BT, FALLING);

  xFila = xQueueCreate(5, sizeof(int));
  if (xFila == NULL)
  {
    Serial.println("Não foi possível criar uma fila");
  }

  xReturned = xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, &task1Handle);
  if (xReturned == pdFAIL)
  {
    Serial.println("Não foi possível criar a task 1");
    while (1)
      ;
  }

  xReturned = xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &task2Handle);
  if (xReturned == pdFAIL)
  {
    Serial.println("Não foi possível criar a task 2");
    while (1)
      ;
  }

  xReturned = xTaskCreate(vTask3, "TASK3", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &task3Handle);
  if (xReturned == pdFAIL)
  {
    Serial.println("Não foi possível criar a task 3");
    while (1)
      ;
  }
}

void loop()
{
  vTaskDelay(3000);
}

void vTask1(void *pvParameters)
{
  // UBaseType_t uxHighWaterMark;
  while (1)
  {
    digitalWrite(LED, !digitalRead(LED));
    vTaskDelay(pdMS_TO_TICKS(200));

    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print(pcTaskGetName(NULL));
    // Serial.print(": ");
    // Serial.println(uxHighWaterMark);
  }
}

void vTask2(void *pvParameters)
{
  int cont = 0;
  while (1)
  {
    Serial.print("Cont: ");
    Serial.println(cont);

    if (cont <= 10)
    {
      xQueueSendToBack(xFila, &cont, portMAX_DELAY);
      cont++;
    }
    else
    {
      cont = 0;
      vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vTask3(void *pvParameters)
{
  int valorRecebido = 0;
  while (1)
  {
    if (xQueueReceive(xFila, &valorRecebido, pdMS_TO_TICKS(1000)) == pdTRUE)
    {
      if (valorRecebido >= 10)
      {
        if (task2Handle != NULL)
        {
          Serial.println("Suspendendo Tasks");
          vTaskSuspend(task2Handle);
          vTaskDelay(pdMS_TO_TICKS(5000));
          Serial.println("Reiniciando Tasks");
          vTaskResume(task2Handle);
        }
      }
    }
  }
}