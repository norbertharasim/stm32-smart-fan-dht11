# STM32 Smart Fan Controller (DHT11 + PWM)

Projekt inteligentnego sterownika wentylatora opartego na mikrokontrolerze **STM32F4** (ARM Cortex-M4). Układ mierzy temperaturę i wilgotność powietrza za pomocą czujnika **DHT11**, przesyła telemetrię przez **UART**, a w kolejnym etapie steruje obrotami wentylatora komputerowego za pomocą sprzętowego sygnału **PWM 25 kHz**.

---

## 🛠 Hardware & Narzędzia
* **MCU:** STM32F4 (rdzeń taktowany zegarem 168 MHz)
* **Czujnik:** DHT11 (temperatura i wilgotność)
* **Docelowy element wykonawczy:** Wentylator 4-pin PWM (standard PC Intel)
* **Narzędzia:** CLion, CMake, STM32CubeMX, GNU Arm Toolchain, PuTTY

---

## 🔌 Połączenie sprzętowe (Wiring)

### Czujnik DHT11
| Moduł DHT11 | Pin STM32 / Zasilanie | Funkcja | Uwagi |
| :--- | :--- | :--- | :--- |
| **+ / VCC** | **5V** | Zasilanie | Stabilna praca układu (3.5V – 5.5V) |
| **- / GND** | **GND** | Masa układu | Wspólna masa |
| **DATA / OUT** | **PA1** | 1-Wire Data | Linia danych z dynamicznym przełączaniem kierunku |

> ⚠️ **Rezystor Pull-Up:** Pomiędzy linią **DATA (PA1)** a zasilaniem **5V (VCC)** wpięty jest rezystor podciągający **4.7 kΩ – 10 kΩ**, który stabilizuje stan wysoki magistrali w fazie spoczynku i odbioru danych (wymagany przy braku wbudowanego rezystora na module).

##  Aktualny status projektu 
- [x] Konfiguracja taktowania rdzenia na 168 MHz w CubeMX.
- [x] Implementacja precyzyjnego opóźnienia mikrosekundowego (`delay_us`) opartego o licznik cykli rdzenia **DWT (Data Watchpoint and Trace)**.
- [x] Opracowanie sterownika czujnika DHT11 (generowanie impulsu startowego, odbiór 40 bitów, weryfikacja sumy kontrolnej).
- [x] Transmisja danych telemetrycznych przez USART2 (115200 bps) do PuTTY.

---

## 🗺 Następne kroki
- [ ] Konfiguracja Timera (TIM1) w trybie generowania sprzętowego sygnału PWM o częstotliwości 25 kHz.
- [ ] Implementacja algorytmu regulacji obrotów w zależności od odczytanej temperatury.
- [ ] Zabezpieczenie termiczne i raportowanie prędkości wentylatora.