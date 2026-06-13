[English](retrospective.md) | [한국어](retrospective.ko.md)

# Retrospective — Judges' Q&A and Troubleshooting

Questions received after submitting to the Nano Day competition, how we responded at
the time, and troubleshooting notes.

## Judges' Q&A

**Q1. Why can't it classify paper?**
Identifying material directly requires a spectroscopic sensor that fires light at a
specific frequency and classifies by reflection characteristics — priced far beyond
an undergraduate project budget. As an alternative we investigated an image sensor +
CNN classifier and confirmed that distinguishing paper from PET bottles was feasible,
but the motor-control integration that would act on the classification result did not
work, so it was excluded from the final system.

**Q2. The shape of incoming items isn't always the same, is it?**
Correct. The biggest shortcoming of this prototype is that the structure only handles
one shape reliably. We concluded that the problem could be solved by passing each item
through a compactor immediately after input to standardize its form before it travels
down the line.

**Q3. What about plastic cans that only have a metal top?**
Depending on the orientation when the item falls, the metal portion may or may not
pass in front of the proximity sensor — so in practice they are sorted randomly. This
cannot be resolved with the current sensor configuration; it would require users to
pre-separate them, or a vision-based classifier.

**Q4. A container with liquid left inside would register as glass, wouldn't it?**
Yes. This is an inherent limitation of weight-based sorting and would have to be
addressed with machine-learning (vision) classification.

**Q5. Can't it handle large PET bottles?**
Not with the current structure. We answered that if the system were scaled up
commercially, adding separate input paths by size would address this.

**Q6. Can't you put in several items at once?**
The conveyor belt was designed to serialize items by dropping them one at a time.
Making the belt longer would let a user load several items and walk away. However,
we were unable to finish tuning the drop timing within the project schedule, so at
the competition we demonstrated with one item inserted at a time.

**Q7. Didn't you convert the weight into kg or g units?**
We did not. The load-cell scale factor (23600) was not calibrated against a reference
weight, so the thresholds 0.5 / 1.0 were relative values. For the standard calibration
procedure, see the calibration section in [code-review.md](code-review.md).

## Troubleshooting Notes

### Servo jitter
After a conditional branch, the servos vibrated severely. We worked around it by
inserting 1-second delays between motions and returning to the home position every
cycle (the chain of delays in the Stage 3 code is the trace of that workaround).
Based on research at the time, the likely root cause was insufficient current and a
missing common GND, both caused by driving MG996R-class servos from the Uno's 5 V pin. The
standard fix is an external power supply, a common GND, and a decoupling capacitor
on the power rail.

### Stage 2 ultrasonic sensor removed
We designed an ultrasonic gate to detect items entering the metal-separation stage
(reflected in the draft code), but it was removed from the final build due to
mechanical interference with the enclosure.

### CNN classifier attempt abandoned
We confirmed that an image sensor could distinguish paper from PET bottles, but the
integration that would drive the motor based on the classification result did not
work, so we dropped it before the competition deadline.
