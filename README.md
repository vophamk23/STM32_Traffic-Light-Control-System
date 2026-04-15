# 🚦 Traffic Light Controller System
![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Proteus](https://img.shields.io/badge/Proteus-1C79C0?style=for-the-badge&logo=proteus&logoColor=white)
### **Intelligent Traffic Management with Cooperative Scheduler**

<div align="center">

<!-- Insert Proteus simulation image here -->
![Traffic Light Proteus Simulation](Report/Traffic%20Light%20System.png)

**Complete traffic light control system using STM32F103C6 with Cooperative Scheduler**

*Built on STM32F103C6 & Simulated in Proteus*

[![STM32](https://img.shields.io/badge/STM32-F103C6-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c6.html)
[![HAL](https://img.shields.io/badge/HAL-Library-green.svg)](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)
[![Proteus](https://img.shields.io/badge/Simulation-Proteus-orange.svg)](https://www.labcenter.com/)

</div>


## 🎯 Overview

This project implements a **smart traffic light control system** for a 2-way intersection using the **STM32F103C6** microcontroller. The system uses a Cooperative Scheduler for efficient task management without RTOS, an FSM (Finite State Machine) for traffic light control, and supports manual time adjustment.

### Key Features:
- **Cooperative Scheduler with O(1) complexity** - Ultra-fast timer interrupt updates
- **4-mode FSM** - 1 automatic mode + 3 manual adjustment modes
- **7-segment display** - Real-time countdown
- **3 control buttons** - MODE, MODIFY, SET
- **Automatic time constraints** - `RED = GREEN + AMBER`
- **Proteus Simulation** - Complete simulation for testing

## 🏗️ System Architecture
 
```
TIM2 IRQ (10ms)
    └─> SCH_Update()          ← O(1), touches only task[0].Delay
 
Main Loop
    └─> SCH_Dispatch_Tasks()  ← executes ready tasks
 
        ├─ Task 1: Button Scan   (every 10ms)
        ├─ Task 2: Traffic FSM   (every 10ms)
        └─ Task 3: Display Update (every 50ms)
```

## Features

### 4 Operating Modes:

**MODE 1 - NORMAL (Automatic):**
- Traffic lights automatically cycle
- Cycle: RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED
- Countdown display on 7-segment LED
- Default timing: RED=5s, GREEN=3s, AMBER=2s

**MODE 2 - RED MODIFY (Adjust RED light):**
- RED LEDs blink (500ms on/off)
- MODIFY button: Increment value (1-99)
- SET button: Save and auto-adjust GREEN

**MODE 3 - AMBER MODIFY (Adjust AMBER light):**
- AMBER LEDs blink
- MODIFY button: Increment value (1-99)
- SET button: Save and auto-adjust RED, GREEN

**MODE 4 - GREEN MODIFY (Adjust GREEN light):**
- GREEN LEDs blink
- MODIFY button: Increment value (1-99)
- SET button: Save and auto-adjust RED

### Control Buttons:
- **Button 1 (MODE)**: Switch between modes (1→2→3→4→1)
- **Button 2 (MODIFY)**: Increment time value in adjustment modes
- **Button 3 (SET)**: Save value and return to automatic mode

### Display:
- **4 × 7-segment LEDs** (2 roads × 2 digits)
- **6 traffic LEDs** (3 colors × 2 roads)
- Scan frequency: 10ms/digit = 40ms full cycle


## System Architecture

```
┌─────────────────────────────────────────────────────┐
│           TIMER2 IRQ (10ms) - O(1)                  │
│              SCH_Update()                            │
└───────────────────┬─────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────┐
│          MAIN LOOP - SCH_Dispatch_Tasks()           │
└───────────────────┬─────────────────────────────────┘
                    │
        ┌───────────┼───────────┐
        │           │           │
        ▼           ▼           ▼
  ┌─────────┐ ┌─────────┐ ┌──────────┐
  │ Task 1  │ │ Task 2  │ │  Task 3  │
  │ Button  │ │ Traffic │ │ Display  │
  │  Scan   │ │   FSM   │ │  Update  │
  └────┬────┘ └────┬────┘ └────┬─────┘
       │           │           │
       ▼           ▼           ▼
   Hardware    FSM Logic   LED/7SEG
    Input      Controller   Output
```


### Required Components:

| Component | Quantity | Description |
|-----------|----------|-------------|
| STM32F103C6T6 | 1 | Main microcontroller |
| 7-segment LED | 4 | Countdown display (multiplexed) |
| LED (Red) | 2 | Red traffic lights |
| LED (Amber) | 2 | Amber traffic lights |
| LED (Green) | 2 | Green traffic lights |
| Push buttons | 3 | MODE, MODIFY, SET |
| 220Ω Resistors | 6 | Current limiting for traffic LEDs |
| 10kΩ Resistors | 3 | Pull-up for buttons |


## State Transition Diagrams

### FSM Modes:

```
       MODE 1              MODE 2              MODE 3              MODE 4
      (NORMAL)           (RED MODIFY)       (AMBER MODIFY)      (GREEN MODIFY)
         │                    │                   │                   │
         │◄───────────────────┴───────────────────┴───────────────────┘
         │                    │                   │                   │
    [MODE btn]           [MODE btn]          [MODE btn]          [MODE btn]
         │                    │                   │                   │
         └───────────────────►└──────────────────►└──────────────────►│
                                                                       │
                                                            [SET or MODE btn]
                                                                       │
                                                                       └──► MODE 1
```

### Traffic States (Mode 1):
```
    INIT
     │
     ▼
┌─────────────┐
│  RED_GREEN  │  Road 1: RED (5s)    Road 2: GREEN (5s)
│   (State 1) │
└──────┬──────┘
       │ Road 2 green ends
       ▼
┌─────────────┐
│  RED_AMBER  │  Road 1: RED (2s)    Road 2: AMBER (2s)
│   (State 2) │
└──────┬──────┘
       │ Road 2 amber ends
       ▼
┌─────────────┐
│  GREEN_RED  │  Road 1: GREEN (3s)  Road 2: RED (3s)
│   (State 3) │
└──────┬──────┘
       │ Road 1 green ends
       ▼
┌─────────────┐
│  AMBER_RED  │  Road 1: AMBER (2s)  Road 2: RED (2s)
│   (State 4) │
└──────┬──────┘
       │ Repeat
       └───────► RED_GREEN
```

## 🔬 Test Cases

### Test Case 1: System Startup
```
GIVEN: System just powered on
WHEN: main() is called
THEN:
  - Mode = 1 (NORMAL)
  - State = INIT → RED_GREEN
  - Road 1: RED on, counter = 5
  - Road 2: GREEN on, counter = 5
  - 7-seg displays "05 05"
```

### Test Case 2: Automatic State Transition
```
GIVEN: Mode 1, State = RED_GREEN, counter_road2 = 1
WHEN: 1 second passes (100 Updates)
THEN:
  - State = RED_AMBER
  - Road 1: RED on, counter = 2
  - Road 2: AMBER on, counter = 2
  - 7-seg displays "02 02"
```

### Test Case 3: Enter Mode 2
```
GIVEN: Mode 1 running
WHEN: Press MODE button
THEN:
  - Mode = 2 (RED_MODIFY)
  - RED LEDs on both roads blink
  - temp_duration = 5 (current RED value)
  - 7-seg displays "02 05"
```

### Test Case 4: Increment Value in Mode 2
```
GIVEN: Mode 2, temp_duration = 5
WHEN: Press MODIFY button 3 times
THEN:
  - temp_duration = 8
  - 7-seg displays "02 08"
```

### Test Case 5: Save and Auto-adjust
```
GIVEN: Mode 2, temp_duration = 10, duration_AMBER = 2
WHEN: Press SET button
THEN:
  - duration_RED = 10
  - duration_GREEN = 10 - 2 = 8  (auto-calculated)
  - Mode = 1 (NORMAL)
  - State = INIT
```

### Test Case 6: Button Edge Detection
```
GIVEN: MODE button held down (currState = PRESS)
WHEN: prevState = PRESS, currState = PRESS
THEN:
  - NO event processing (prevents multiple triggers)
```

## License

This project is part of coursework and provided for educational purposes.


## Author

**vophamk23**
- GitHub: [@vophamk23](https://github.com/vophamk23)
- University: Ho Chi Minh City University of Technology (HCMUT)
- Course: Microprocessor and Microcontroller

---

## Acknowledgments

- **HCMUT** - Course materials and lab facilities
- **STMicroelectronics** - HAL library and STM32 documentation
- **Labcenter Electronics** - Proteus simulation software
- Instructors - Guidance and support

---

## References

### Datasheets:
- [STM32F103C6 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c6.pdf)
- [STM32F1 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf)
- [STM32F1 HAL User Manual](https://www.st.com/resource/en/user_manual/um1850.pdf)

---

<div align="center">

### ⭐ If this project helped you, please give it a star! ⭐

**Built with ❤️ for embedded systems learning**

![Status](https://img.shields.io/badge/Status-Working-success.svg)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)

</div>
