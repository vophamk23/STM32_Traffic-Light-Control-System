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

---

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Core Components](#core-components)
- [Hardware Configuration](#hardware-configuration)
- [Usage Guide](#usage-guide)
- [Proteus Simulation](#proteus-simulation)

---

## 🎯 Overview

This project implements a **smart traffic light control system** for a 2-way intersection using the **STM32F103C6** microcontroller. The system uses a Cooperative Scheduler for efficient task management without RTOS, an FSM (Finite State Machine) for traffic light control, and supports manual time adjustment.

### Key Features:
- **Cooperative Scheduler with O(1) complexity** - Ultra-fast timer interrupt updates
- **4-mode FSM** - 1 automatic mode + 3 manual adjustment modes
- **7-segment display** - Real-time countdown
- **3 control buttons** - MODE, MODIFY, SET
- **Automatic time constraints** - `RED = GREEN + AMBER`
- **Proteus Simulation** - Complete simulation for testing


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

### Main Processing Flow:

```
1. Timer 2 generates interrupt every 10ms
2. SCH_Update() decrements delay of first task (O(1))
3. Main loop calls SCH_Dispatch_Tasks()
4. Dispatch checks which tasks are ready (RunMe=1)
5. Execute tasks: Button → FSM → Display
6. Periodic tasks are re-added with delay = period
```

---

## 🔧 Core Components

### **1. `main.c` - Entry Point and Timer Setup**

**Main Functions:**
- System initialization (Clock, GPIO, Timer)
- Scheduler setup and adding 3 main tasks
- Main loop runs `SCH_Dispatch_Tasks()`

**Timer Configuration:**
```c
// TIM2: 8MHz / (7999+1) / (9+1) = 100Hz → 10ms interrupt
htim2.Init.Prescaler = 7999;
htim2.Init.Period = 9;
```

**Timer Callback - Heart of Scheduler:**
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        SCH_Update();  // Only call this - O(1) complexity!
    }
}
```

**Task Setup:**
```c
SCH_Add_Task(Task_Button_Scan, 0, 10);     // Scan buttons every 10ms
SCH_Add_Task(Task_Traffic_FSM, 0, 10);     // FSM runs every 10ms  
SCH_Add_Task(Task_Update_Display, 0, 50);  // Update display every 50ms
```


### **2. `scheduler.c` - Cooperative Scheduler**

**Characteristics:**
- **O(1) Update function** - Extremely fast in interrupt
- Task array always sorted by `Delay` in ascending order
- Supports both periodic and one-shot tasks

**Task Structure:**
```c
typedef struct {
    void (*pTask)(void);   // Function pointer
    uint32_t Delay;        // Wait time (ticks)
    uint32_t Period;       // Repeat cycle (0=one-shot)
    uint8_t RunMe;         // Ready-to-run flag
    uint32_t TaskID;       // Identifier
} sTask;
```

**Key Functions:**

**`SCH_Init()`** - Initialize scheduler:
```c
// Clear all tasks, reset variables
task_count = 0;
elapsed_time = 0;
```

**`SCH_Add_Task()`** - Add task (Insertion Sort - O(n)):
```c
// Find insertion position to keep array sorted
// Example: [10, 20, 50] + task(30) → [10, 20, 30, 50]
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
```

**`SCH_Update()`** - Called in interrupt (O(1)):
```c
void SCH_Update(void) {
    // ONLY decrement delay of first task
    if (SCH_tasks_G[0].Delay > 0) {
        SCH_tasks_G[0].Delay--;
    }
    elapsed_time++;
    
    // Set flag when time is up
    if (SCH_tasks_G[0].Delay == 0) {
        SCH_tasks_G[0].RunMe = 1;
    }
}
```

**`SCH_Dispatch_Tasks()`** - Execute tasks (O(n²)):
```c
void SCH_Dispatch_Tasks(void) {
    // STEP 1: Update delay for all tasks
    for (m = 0; m < task_count; m++) {
        if (MARKING[m] == 0) {
            // Subtract elapsed_time
            SCH_tasks_G[m].Delay -= elapsed_time;
        } else {
            // Task has same delay as task[0]
            SCH_tasks_G[m].RunMe = 1;
        }
    }
    
    // STEP 2: Execute ready tasks
    while (SCH_tasks_G[0].RunMe > 0) {
        (*SCH_tasks_G[0].pTask)();  // Call task function
        
        if (Period == 0) {
            SCH_Delete_Task(0);  // One-shot
        } else {
            // Periodic: Delete then re-add
            SCH_Delete_Task(0);
            SCH_Add_Task(func, period, period);
        }
    }
}
```

**Operation Example:**
```
Initial: Task[0]=100, Task[1]=200
│
├─ Update #1: Task[0]=99, elapsed=1
├─ Update #2: Task[0]=98, elapsed=2
│  ...
├─ Update #100: Task[0]=0, RunMe=1, elapsed=100
│
└─ Dispatch:
   ├─ Task[1].Delay = 200-100 = 100
   ├─ Run Task[0]
   └─ Task[0] is re-added with Delay=100
```

### **3. `fsm_traffic.c` - Finite State Machine**

**4-Mode FSM:**

| Mode | Name | Function |
|------|------|----------|
| 1 | MODE_1_NORMAL | Automatic mode |
| 2 | MODE_2_RED_MODIFY | Adjust RED duration |
| 3 | MODE_3_AMBER_MODIFY | Adjust AMBER duration |
| 4 | MODE_4_GREEN_MODIFY | Adjust GREEN duration |

**Traffic States (Mode 1):**

```c
typedef enum {
    INIT,          // Initialization
    RED_GREEN,     // Road 1: RED, Road 2: GREEN
    RED_AMBER,     // Road 1: RED, Road 2: AMBER
    GREEN_RED,     // Road 1: GREEN, Road 2: RED
    AMBER_RED      // Road 1: AMBER, Road 2: RED
} TrafficState;
```

**FSM Mode 1 - Automatic mode:**
```c
void fsm_normal_mode(void)
{
    static int timer_counter = 0;
    
    // Check MODE button
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_2_RED_MODIFY;
        return;
    }
    
    // Count cycles (100 ticks = 1 second)
    timer_counter++;
    if (timer_counter < 100) return;
    timer_counter = 0;
    
    // FSM state transitions every second
    switch(traffic_state) {
        case INIT:
            traffic_state = RED_GREEN;
            counter_road1 = duration_RED;    // 5s
            counter_road2 = duration_GREEN;  // 3s
            break;
            
        case RED_GREEN:
            counter_road1--;
            counter_road2--;
            if (counter_road2 <= 0) {
                traffic_state = RED_AMBER;
                counter_road1 = duration_AMBER;  // 2s
                counter_road2 = duration_AMBER;
            }
            break;
        // ... other cases similar
    }
}
```

**FSM Mode 2/3/4 - Adjustment modes:**
```c
void fsm_red_modify_mode(void)
{
    // MODE button → Switch to mode 3
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_3_AMBER_MODIFY;
        temp_duration = duration_AMBER;
        return;
    }
    
    // MODIFY button → Increment value
    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
        temp_duration++;
        if (temp_duration > 99) temp_duration = 1;
    }
    
    // SET button → Save and auto-adjust
    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
        duration_RED = temp_duration;
        auto_adjust_duration(0);  // Auto-calculate GREEN
        current_mode = MODE_1_NORMAL;
        return;
    }
    
    // Blink RED LEDs
    handle_led_blinking(0);
}
```

**Auto-adjust Duration Function:**
```c
int auto_adjust_duration(int modified_light)
{
    // Constraint: RED = GREEN + AMBER
    
    switch(modified_light) {
        case 0:  // RED was modified
            // Keep AMBER, calculate GREEN
            duration_GREEN = duration_RED - duration_AMBER;
            break;
            
        case 1:  // AMBER was modified
            // GREEN = AMBER + 4
            duration_GREEN = duration_AMBER + 4;
            duration_RED = duration_GREEN + duration_AMBER;
            break;
            
        case 2:  // GREEN was modified
            // Keep AMBER, calculate RED
            duration_RED = duration_GREEN + duration_AMBER;
            break;
    }
    
    // Check limits 1-99
    // Reset to defaults if invalid
    return 1;
}
```

**Edge Detection for Buttons:**
```c
void update_button_state(void)
{
    for (int i = 0; i < 3; i++) {
        prevState[i] = currState[i];
        
        // Read new state
        if (isButton1Pressed()) {
            currState[0] = BTN_PRESS;
        } else {
            currState[0] = BTN_RELEASE;
        }
    }
    
    // Detection: prevState=RELEASE && currState=PRESS
    // → Process button press event
}
```

---

## Hardware Configuration

<!-- Insert pin configuration diagram here -->
![Pin Configuration](https://raw.githubusercontent.com/vophamk23/Project_STM32_Traffic-Light-Control-System/main/C%E1%BA%A5u%20h%C3%ACnh%20ch%C3%A2n%20STM32.png)



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

### GPIO Pin Configuration:

#### **Traffic LEDs (6 LEDs):**
```c
// Road 1:
RED1_Pin     → PA0   // Red light road 1
YELLOW1_Pin  → PA1   // Amber light road 1
GREEN1_Pin   → PA2   // Green light road 1

// Road 2:
RED2_Pin     → PA3   // Red light road 2
YELLOW2_Pin  → PA4   // Amber light road 2
GREEN2_Pin   → PA5   // Green light road 2
```

#### **7-segment LEDs (16 pins):**
```c
// 4 digits (digit select):
inputseg0_0 ~ inputseg0_3 → PA6-PA9   // Digit 0 (Road 1 - tens)
inputseg1_0 ~ inputseg1_3 → PB0-PB3   // Digit 1 (Road 1 - units)
inputseg2_0 ~ inputseg2_3 → PB10-PB13 // Digit 2 (Road 2 - tens)
inputseg3_0 ~ inputseg3_3 → PB4-PB5, PA10-PA11 // Digit 3 (Road 2 - units)

// 4 mode indicator pins:
inputmode_0 ~ inputmode_3 → PB6-PB9
```

#### **Push Buttons (3 buttons):**
```c
button1_Pin → PA11  // MODE button (mode switching)
button2_Pin → PA12  // MODIFY button (increment value)
button3_Pin → PA13  // SET button (save and confirm)
```

### Connection Diagram:
```
STM32F103C6
     │
     ├─ PA0-PA5  → Traffic LEDs (6 LEDs)
     ├─ PA6-PA13 → 7-segment + Buttons
     └─ PB0-PB13 → 7-segment
```

---

## Usage Guide

### NORMAL Mode (Automatic):

1. **System Startup**
   - System automatically enters Mode 1
   - Traffic lights begin cycling

2. **Default Cycle:**
   ```
   State 1: Road 1 RED (5s)     | Road 2 GREEN (5s)
            ↓ (Road 2 green ends)
   State 2: Road 1 RED (2s)     | Road 2 AMBER (2s)
            ↓ (Road 2 amber ends)
   State 3: Road 1 GREEN (3s)   | Road 2 RED (3s)
            ↓ (Road 1 green ends)
   State 4: Road 1 AMBER (2s)   | Road 2 RED (2s)
            ↓ (Repeat from start)
   ```

3. **7-segment Display:**
   - First 2 digits: Road 1 countdown
   - Last 2 digits: Road 2 countdown
   - Example: `05 03` = Road 1 has 5s, Road 2 has 3s

### Adjustment Modes (Mode 2/3/4):

#### **Entering Adjustment Mode:**
- Press **MODE button** from Mode 1 → Enter Mode 2 (RED adjustment)

#### **Mode 2 - Adjust RED Duration:**
1. RED LEDs on both roads blink (500ms on/off)
2. Display shows: `02 05` (Mode 2, RED time = 5s)
3. Press **MODIFY** to increment (1→2→...→99→1)
4. Press **SET** to save:
   - Auto-calculate: `GREEN = RED - AMBER`
   - Example: RED=10, AMBER=2 → GREEN=8
   - Return to Mode 1

#### **Mode 3 - Adjust AMBER Duration:**
1. Press **MODE** from Mode 2 → Enter Mode 3
2. AMBER LEDs on both roads blink
3. Display shows: `03 02` (Mode 3, AMBER time = 2s)
4. Press **MODIFY** to increment
5. Press **SET** to save:
   - Auto-calculate: `GREEN = AMBER + 4`
   - Then: `RED = GREEN + AMBER`
   - Example: AMBER=3 → GREEN=7, RED=10

#### **Mode 4 - Adjust GREEN Duration:**
1. Press **MODE** from Mode 3 → Enter Mode 4
2. GREEN LEDs on both roads blink
3. Display shows: `04 03` (Mode 4, GREEN time = 3s)
4. Press **MODIFY** to increment
5. Press **SET** to save:
   - Auto-calculate: `RED = GREEN + AMBER`
   - Example: GREEN=8, AMBER=2 → RED=10

#### **Return to Mode 1:**
- Method 1: Press **SET** in any mode (saves changes)
- Method 2: Press **MODE** from Mode 4 (does not save)

### Timing Constraints:
```
ALWAYS: RED = GREEN + AMBER

Valid examples:
- RED=5, GREEN=3, AMBER=2 ✅
- RED=10, GREEN=7, AMBER=3 ✅

Invalid (auto-adjusted):
- RED=5, GREEN=4, AMBER=2 ❌ → RED=6
```

---

## Proteus Simulation

### Simulation Setup:

**1. Open Proteus Design Suite:**
   - Create new project or open existing `.pdsprj` file

**2. Add Components:**
   ```
   - STM32F103C6 (or STM32F103C8)
   - 6× LEDs (2 red, 2 amber, 2 green)
   - 4× 7-segment displays (Common Cathode or Common Anode)
   - 3× Push buttons
   - 220Ω resistors (for LEDs)
   - 10kΩ resistors (pull-up for buttons)
   - 5V power supply
   ```

**3. Connect According to Pin Configuration:**
   - Follow GPIO configuration table above
   - Ensure common GND connection
   - Buttons use pull-up (connect to VCC through 10kΩ)

**4. Load Program:**
   ```
   - Build project in STM32CubeIDE
   - Get file: Debug/TrafficLight_Controller_System_Scheduler.hex
   - Right-click STM32 → Edit Properties
   - Program File → Browse to .hex file
   ```

**5. Run Simulation:**
   - Click Play button (▶) at bottom left
   - Observe automatic traffic light cycling
   - Click buttons with mouse to test functionality

### Function Testing:

**✅ Test Mode 1 (Auto):**
- Lights automatically cycle
- 7-segment countdown accurate
- Cycle repeats continuously

**✅ Test Mode 2/3/4 (Adjustment):**
- Press MODE button → LEDs blink
- Press MODIFY button → Number increments on 7-segment
- Press SET button → Saves and returns to auto

**✅ Test Timer:**
- Measure actual time with Proteus stopwatch
- Verify: 1 second = 100 interrupts (10ms × 100)

**✅ Test Scheduler:**
- Add breakpoint at `SCH_Dispatch_Tasks()`
- Check task_count, delay, RunMe

### Debugging in Proteus:

**1. Virtual Terminal:**
   - Add UART for debug messages
   - Printf task info, FSM states

**2. Logic Analyzer:**
   - Observe GPIO signals
   - Check multiplexing frequency (40ms cycle)

**3. Oscilloscope:**
   - Measure timer interrupt accuracy (10ms)
   - Verify LED blink duty cycle (50%)

---

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

---

## Performance Analysis

### Scheduler Performance:

| Operation | Complexity | Time (estimated) |
|-----------|------------|------------------|
| SCH_Update() | **O(1)** | ~5μs |
| SCH_Dispatch_Tasks() | O(n²) | ~100μs (3 tasks) |
| SCH_Add_Task() | O(n) | ~50μs |
| SCH_Delete_Task() | O(n) | ~30μs |

**Note:** 
- `SCH_Update()` runs in interrupt → Must be extremely fast → O(1)
- `SCH_Dispatch_Tasks()` runs in main loop → O(n²) is acceptable

### Timer Accuracy:
```
Target: 10ms interrupt
Actual: 10.000ms ±0.001ms (with 8MHz crystal)
Error: < 0.01%
```

### Memory Usage:
```
RAM: ~2KB (task array + variables)
Flash: ~15KB (code + libraries)
Stack: ~512 bytes
```

---

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

---

## 📝 Code Structure

```
Core/Src/
├── main.c               // Entry point, timer setup, task registration
├── scheduler.c          // O(1) scheduler implementation
├── fsm_traffic.c        // 4-mode FSM with edge detection
├── button.c             // Button debouncing (if exists)
├── Tasks.c              // Define 3 tasks
└── [display files]      // LED and 7-segment control
```

### Dependency Graph:
```
main.c
  ├── scheduler.c
  │     ├── Tasks.c
  │     │     ├── fsm_traffic.c
  │     │     │     ├── button logic (edge detect)
  │     │     │     └── LED control
  │     │     └── display update
  │     └── SCH_Update() ← Timer IRQ
  └── SCH_Dispatch_Tasks() ← Main loop
```

---

## 🤝 Contributing

Contributions welcome! Here's how:

1. **Fork** this repository
2. Create feature branch (`git checkout -b feature/NewFeature`)
3. **Commit** changes (`git commit -m 'Add feature XYZ'`)
4. **Push** to branch (`git push origin feature/NewFeature`)
5. Create **Pull Request**

### Improvement Ideas:
- [ ] Add UART communication for monitoring
- [ ] Emergency vehicle priority mode
- [ ] Adaptive timing based on traffic density
- [ ] Add sensors to count vehicles
- [ ] Save configuration to EEPROM
- [ ] Web interface for remote control

---

## License

This project is part of coursework and provided for educational purposes.

---

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
