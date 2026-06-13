[English](code-review.md) | [한국어](code-review.ko.md)

# Code Review — Technical Analysis and Improvement Proposals

A post-mortem analysis of the competition code (`firmware/original/`). Changes that
were applied in `firmware/revised/` are distinguished from improvements left as
proposals only.

## Per-Sketch Analysis

### Stage 1 — stage1_conveyor

**Behavior**: Two ultrasonic sensors measure distance; if both detect an object within
their respective thresholds (10 cm / 20 cm), the servo goes to 0 (belt running);
if either is out of range, the servo goes to 90 (stop for a continuous-rotation servo).

| # | Issue | Resolution |
|---|---|---|
| 1 | `pulseIn(echo, HIGH)` has no timeout — on echo loss it blocks for the default 1 second. With two sensors in series, one cycle can stall for 2+ seconds, the likely cause of "the motor feels unresponsive" | revised: 30 ms timeout + keep belt state on failure |
| 2 | Single-shot measurement exposes the system to noise; one spurious reading can jerk the belt | proposal: 3-sample median filter |
| 3 | Continuous-rotation servos have a deadband that shifts slightly per unit — `write(90)` may still creep slowly | proposal: use `writeMicroseconds()` to trim the stop pulse width |
| 4 | `int pos = 0;` unused variable | revised: removed |

### Stage 2 — stage2_metal_sorter (draft)

**3 compile errors (with compound causes)** — why this draft does not compile:

| Location | Code | Problem |
|---|---|---|
| `setup()` | `pinMode(senor_pin, INPUT)` | `senor_pin` undeclared (only `#define senor 4` exists) |
| `setup()` | `tap_servo.attach(tap_servo_pin)` | `tap_servo` is a `#define tap_servo 5` pin macro — `.attach()` cannot be called on it; `tap_servo_pin` is also undeclared |
| `loop()` | `digitalRead(sensor)` | `sensor` undeclared (typo of `senor`) |

The root cause is the same object being named five different ways: `senor` / `sensor` /
`senor_pin` / `tap_servo` / `tap_servo_pin`. In revised, pin numbers were unified as
`SENSOR_PIN` / `TAP_SERVO_PIN` constants and the servos split into `doorServo` /
`tapServo` objects.

**Logic issues**:
1. On measurement failure (echo timeout), `duration=0 → distance=0 < 10`, so the
   **door stays open**. A failure mode of "open" is not safe. → revised: keep door
   closed on timeout
2. `if (val==0) {...} if (val==1) {...}` is functionally equivalent to if/else but
   the intent is not clear. → revised: changed to if/else
3. For inductive proximity sensors, the meaning of `val==0` flips depending on output
   type (NPN: LOW when metal detected / PNP: HIGH when metal detected), and 24 V
   industrial sensors need a voltage divider or level shifter. Wiring documentation
   specific to the module used should have been preserved alongside the code.

### Stage 3 — stage3_weight_sorter

**Record state**: The source was cut off inside `loop()`, losing the closing brace →
restored in revised.

Note: Stage 3 runs Serial at 38400 baud, unlike Stages 1 and 2 (115200). Remember
to set each board's Serial Monitor to the correct baud rate.

| # | Issue | Resolution |
|---|---|---|
| 1 | `scale.get_units(5)` blocks for ~0.5 s at HX711's default 10 SPS. The heavy-item path adds 5 more `delay(1000)` calls, making one cycle ~5.6 s — a hard ceiling on throughput | proposal: reduce sample count or enable 80 SPS mode; refactor to state machine |
| 2 | Chained `delay(1000)` was a deliberate buffer for servo jitter (author's note). Root cause is likely the power supply — MG996R-class servos can draw ~2.5 A at stall, exceeding the Uno's 5 V regulator | proposal: external 5–6 V supply + common GND + 470–1000 µF capacitor |
| 3 | Scale factor 23600 was set without a calibration weight, so the measurement unit is undefined (judges' Q7) | proposal: ① `tare()` with empty scale ② place reference/calibration weight (e.g. 1.000 kg) ③ adjust factor until `get_units()` reads 1.000 |
| 4 | Readings ≤ 0.5 match no branch, so the item **is never ejected**. Given that empty PET bottles (tens of grams) are a primary target, the threshold validity is unconfirmed without calibration | revised: kept original behavior, added comment noting the limitation |

## Architecture Analysis

The three Unos **operate independently with no inter-board communication**. There is
no inter-stage synchronization, so if an item arrives while the previous stage is
still processing, a collision can occur; this was avoided by padding each stage with
long delays. It worked, but it was a trade-off that sacrificed both throughput and
reliability.

## Improvement Roadmap (priority order)

1. **Remove delays — `millis()` state machine**: Restructuring each sketch into
   idle / measure / act states allows sensor polling and servo motion to overlap,
   restoring responsiveness.
2. **Separate servo power supply**: Prerequisite for removing the jitter-buffer
   delays (Issue 2 in Stage 3 above).
3. **Inter-board handshake**: Sharing even a single digital pin as a busy signal
   eliminates inter-stage collisions. Going further, the three Unos could be
   consolidated into a single Mega, or linked via I2C master–slave.
4. **Paper classification — low-cost vision**: Mounting a lightweight CNN trained
   with TensorFlow Lite Micro or Edge Impulse on an ESP32-CAM (~$5) enables
   paper / PET classification without a spectroscopic sensor. The problem at the time
   — "image sensor could classify, but motor integration didn't work" — can be solved
   by separating the classification board from the drive board and passing only the
   classification result over serial.
5. **Shape normalization**: A compactor immediately after the input (the same
   principle as the compaction stage in industrial sorting plants) structurally
   eliminates shape variance.
