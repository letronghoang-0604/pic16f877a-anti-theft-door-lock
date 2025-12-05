# pic16f877a-anti-theft-door-lock
Anti-theft door lock with keypad and LCD display, siren, remote locking and warning function using PIC16F877A (CCS C)

# Anti-Theft Keypad Door Lock using PIC16F877A (CCS C)

This is a small embedded project using a **PIC16F877A** microcontroller and **CCS C** compiler to implement a keypad-based anti-theft door lock system.  
The system uses a **4x3 matrix keypad** to enter a 4-digit password, displays status on a **16x2 LCD**, controls a **door lock output**, drives a **buzzer** and can trigger a **remote sender output** after multiple wrong attempts. A lockout countdown is shown on the LCD when the user exceeds the allowed number of failed password attempts. :contentReference[oaicite:1]{index=1}

## Features

- **4x3 keypad password input**:
  - Fixed 4-character password (default: `1 2 3 #`).
  - Each key press is echoed on the LCD.
- **Door lock control**:
  - Door is locked by default.
  - Correct password unlocks the door for a fixed period (e.g. 5 seconds) and then relocks automatically.
- **Wrong attempt counter and lockout**:
  - Counts consecutive wrong passwords.
  - After **3 wrong attempts**, the system:
    - Activates the buzzer.
    - Pulses a **sender output** (e.g. RF module / alarm).
    - Enters a **60-second lockout** period where new attempts are blocked.
- **LCD user interface**:
  - Shows messages like:
    - `"CUA DA KHOA"` (Door locked)
    - `"CUA DA MO"` (Door opened)
    - `"MAT KHAU SAI"` (Wrong password)
    - `"THU LAI SAU xx GIAY"` (Try again after xx seconds)
- **Door open input (manual override or sensor)**:
  - A door open input (`MOCUA`) can be used to detect when the door is opened and display the corresponding status.

## Hardware

- Microcontroller: **PIC16F877A** (20 MHz external crystal)
- 4x3 matrix keypad:
  - 4 rows, 3 columns
- 16x2 character LCD (4-bit mode) using `lcd.c`
- Buzzer (for alarm and feedback)
- Door lock output (relay or transistor driver, `KHOACUA`)
- Sender output (to drive an RF module or external alarm, `SENDER`)
- Door open input / button (`MOCUA`)
- Power supply (5 V for logic, plus supply for relay if needed)
- Miscellaneous: resistors, wires, breadboard or PCB

Typical pin usage (based on the example code):

- Buzzer: `PIN_A2`
- Door lock control (KHOACUA): `PIN_A1`
- Sender output: `PIN_A0`
- Door open input (MOCUA): `PIN_E2`
- Keypad rows: `PIN_D2`, `PIN_D1`, `PIN_D0`, `PIN_C3`
- Keypad columns: `PIN_C2`, `PIN_C1`, `PIN_C0`
- LCD:
  - `LCD_ENABLE_PIN` → `PIN_B2`
  - `LCD_RS_PIN`     → `PIN_B4`
  - `LCD_RW_PIN`     → `PIN_B3`
  - `LCD_DATA4`      → `PIN_D7`
  - `LCD_DATA5`      → `PIN_D6`
  - `LCD_DATA6`      → `PIN_D5`
  - `LCD_DATA7`      → `PIN_D4`

## Software & Tools

- **Compiler**: CCS C for PIC (`#include <16F877A.h>`)  
- Clock: 20 MHz (`#use delay(crystal=20000000)`)
- LCD library: `lcd.c` (standard CCS LCD driver)
- IDE: CCS IDE or MPLAB X with CCS plug-in
- Programmer: PICkit or compatible programmer for PIC16F877A
- Optional: Proteus for simulation of keypad, LCD and door lock logic

## Code Overview

Main source file: `door_lock_alarm.c` (original name: `test22.c`, placed under the `code/` folder).

Key points:

- **Keypad scanning**:
  - A 4x3 matrix keypad is scanned manually by driving each row low in turn and reading the column inputs.
  - The `keypad()` function sets all row outputs high, then pulls each row low and checks each column input to detect which key is pressed.
  - The `codekeypad[]` array maps key indices to characters: `'1'..'9', '*', '0', '#'`.

- **Password input buffer**:
  - `input_buffer[5]` holds up to 4 characters plus a terminator.
  - `input_index` tracks the number of entered characters.
  - Each key press:
    - Is displayed on the second LCD line.
    - Is stored in `input_buffer`.

- **Password checking**:
  - When `input_index` reaches 4:
    - The input is compared to the fixed password `{'1','2','3','#'}`.
    - If all 4 characters match, `correct = 1` (success).
    - Otherwise, it is a wrong attempt.

- **Correct password handling**:
  - Clear the LCD and show `"CUA DA MO"` (Door opened).
  - Turn OFF the buzzer, turn OFF the lock output (`KHOACUA`) to unlock the door.
  - Wait 5 seconds, then relock the door by turning `KHOACUA` back ON.
  - Reset `wrong_attempts` to 0.

- **Wrong password handling**:
  - Show `"MAT KHAU SAI"` (Wrong password) for a short period.
  - Increment `wrong_attempts`.
  - If `wrong_attempts >= 3`:
    - Set `lockout = 1`.
    - Turn ON the buzzer for a few seconds.
    - Pulse the `SENDER` pin (e.g. send a short alarm signal).
  - After handling, the LCD is restored to `"CUA DA KHOA"` and the input buffer is cleared.

- **Lockout countdown**:
  - When `lockout` is set, `lockout_countdown()` runs:
    - For 60 seconds, the LCD shows:
      - Line 1: `"THU LAI SAU"`
      - Line 2: `"xx GIAY"` where `xx` counts down from 60.
    - The buzzer beeps briefly each second.
  - After the countdown:
    - `lockout` is cleared.
    - `wrong_attempts` reset to 0.
    - LCD returns to `"NHAP MAT KHAU"` / `"CUA DA KHOA"` prompt.

- **Door open input**:
  - If the `MOCUA` input is active (e.g. a door open button is pressed):
    - The door is unlocked (`KHOACUA` low).
    - LCD shows `"CUA DA MO"` for 5 seconds.
    - Then the door is locked again and the LCD shows `"CUA DA KHOA"`.

## How It Works (High-Level)

1. **Initialize:**
   - Configure TRIS registers for:
     - Port A: buzzer, sender, door lock as outputs; other pins as inputs.
     - Port C/D: keypad rows/columns and LCD pins.
     - Port E: door open input.
   - Initialize the LCD and show `"CUA DA KHOA"` (Door locked).
   - Ensure the door lock output (`KHOACUA`) is active (door locked) and the buzzer and sender are OFF.
   - Initialize variables: `input_index`, `wrong_attempts`, `lockout`.

2. **In the main loop:**
   - If `lockout` is active:
     - Call `lockout_countdown()` to show the 60-second countdown and beep the buzzer.
   - Call `keypad()` to scan for a new key press.
   - Check the door open input (`MOCUA`):
     - If pressed, temporarily unlock the door, show `"CUA DA MO"`, wait, then relock and show `"CUA DA KHOA"`.
   - If a valid key was detected:
     - Display the character on the LCD at the current `input_index` position.
     - Store it in `input_buffer` and increment `input_index`.
     - When 4 characters have been entered:
       - Compare with the correct password.
       - Handle success (unlock door) or failure (increment `wrong_attempts`, possibly enter lockout and trigger buzzer/sender).
       - Clear the input buffer and reset `input_index` for the next attempt.
   - Small delays are used for debouncing and user readability, then the loop repeats continuously.
