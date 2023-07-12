/***********************************************************
 *
 *  Exemplo de criação de task
 *
 ***********************************************************/

#include <Arduino.h>

#define LED 19
#define BTN 12

/* Handles */
TaskHandle_t xTaskTrataBTHandle = NULL;

/* Protótipo das tasks */
void vTaskTrataBTN(void *pvParameters);
void callBackBT(void)
{
  vTaskNotifyGiveFromISR(xTaskTrataBTHandle, nullptr);
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), callBackBT, FALLING);
  xTaskCreate(vTaskTrataBTN, "TASK BTN", configMINIMAL_STACK_SIZE, NULL, 1, &xTaskTrataBTHandle);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void vTaskTrataBTN(void *pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial.println("Tratando ISR BTN.");
    digitalWrite(LED, !digitalRead(LED));
    delay(500);
  }
}