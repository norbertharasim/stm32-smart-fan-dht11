#include "dht11.h"

void DHT11_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

// Szybkie przełączanie kierunku pinu przez bezpośrednią modyfikację rejestrów
static void DHT11_SetPin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

static void DHT11_SetPin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

uint8_t DHT11_Read(DHT11_Data_t *data) {
    uint8_t raw_bytes[5] = {0};
    uint32_t timeout = 0;

    // 1. Sygnał START (MCU wymusza stan niski przez 18-20 ms)
    DHT11_SetPin_Output();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    HAL_Delay(19);
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    delay_us(25);

    // 2. Przełączenie na wejście - oddajemy linię czujnikowi
    DHT11_SetPin_Input();

    // Czekaj na odpowiedź czujnika (ściągnięcie do stanu niskiego)
    timeout = 100000;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) return 1;
    }

    // Czekaj na koniec stanu niskiego czujnika (~80 us)
    timeout = 100000;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET) {
        if (--timeout == 0) return 1;
    }

    // Czekaj na koniec stanu wysokiego czujnika (~80 us)
    timeout = 100000;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) return 1;
    }

    // 3. Odbiór 40 bitów (5 bajtów)
    for (int byte_idx = 0; byte_idx < 5; byte_idx++) {
        for (int bit_idx = 7; bit_idx >= 0; bit_idx--) {
            // Czekaj na zakończenie stanu niskiego poprzedzającego bit (~50 us)
            timeout = 100000;
            while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET) {
                if (--timeout == 0) return 1;
            }

            // DHT11: 26-28 us to '0', a 70 us to '1'
            // Sprawdzamy stan po 35 us
            delay_us(35);

            if (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) {
                raw_bytes[byte_idx] |= (1 << bit_idx);

                // Czekaj aż skończy się stan wysoki dla bitu '1'
                timeout = 100000;
                while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) {
                    if (--timeout == 0) return 1;
                }
            }
        }
    }

    // 4. Weryfikacja sumy kontrolnej
    uint8_t checksum = raw_bytes[0] + raw_bytes[1] + raw_bytes[2] + raw_bytes[3];
    if (checksum != raw_bytes[4]) {
        return 2; // Błąd sumy kontrolnej
    }

    data->humidity = raw_bytes[0];
    data->temperature = raw_bytes[2];

    return 0;
}