# Tutorial 2 Homework - PWM

#### Main Author: Caner Demirer (cdemirer at connect ust hk)

In this homework you will design and implement a "melee attack controller". The setup consists of a development board (which has 3 buttons, 3 LEDs and a buzzer), a servo, and a tiny 3D-printed "melee weapon" attached to the servo. You need to use PWM to control the servo to achieve the desired functionality described below. There will also be a video illustration near the end of this file for your reference.

The main goal is to have a controller that works and feels like those in Action RPG video games. Since having movements as complex as those in the video games is very difficult to achieve with real actuators, this task is very simplified. 

### Details

There are **three(+) types of attack**: 
 - Light Attack
 - Heavy Attack (Chargeable)
 - Special Attack (Designed by you)
 - Some combo attacks defined further below. 
     - After the combo counter hits 3, Special Attack is activated for single use.

The target is imagined to be directly in front. All movement is rotation on a single axis. There are only two attack directions: attack from the left and attack from the right. The attack direction will alternate. 

When there is nothing to do, the weapon stays at Resting Angle of either side. All attacks start at one side's Resting Angle ~with a beep~ and end at the other side's Resting Angle ~with a beep~.

**The buttons are assigned** like this: [Light Attack] [Heavy Attack] [Special Attack]. An attack request is made when the corresponding button is pressed.

**A Light Attack** starts at Resting Angle, moves towards the target and ends at Resting Angle (of the opposite side). 

**A Heavy Attack** has 2 stages of movement. It starts at Resting Angle, moves away from the target any visible amount, pausing there (charging) until the player releases the Heavy Attack button. If the player had already released the button while the Heavy Attack was in queue, then there won't be any charging nor pausing. It then moves towards the target and ends at Resting Angle (of the opposite side).

**A fully-charged Heavy Attack** is a Heavy Attack that has been charged for at least 1 second. 
- A Heavy Attack in the first stage of its movement charges during the time the Heavy Attack button is held. While the Heavy Attack is being charged, it continues moving further away from the target while decelerating (during its initial movement). 
- Once the heavy attack has been fully charged, a double short beep is played
- If the Heavy Attack is a Fully Charged Heavy Attack, before it ends it overshoots the Resting Angle (of the opposite side), and after that comes back to it again.

**A Special Attack** is a custom attack animation that must be designed by yourself, the only requirements are:
- It looks impressive.
- Lasts at least 2 seconds.
- Plays a continuous beep while the attack is going on.
- Starts at one side's resting angle and ends at the other side's resting angle.

**When an attack is requested**, there are a few possible things that can happen: 
- If there is currently nothing to do, the attack starts immediately. 
- If there is an attack in progress that hasn't hit the target yet, or has hit the target but an attack has already been queued, then the requested attack is ignored. 
- If there is an attack in progress that has hit the target and no attack has been queued yet, then the requested attack is queued. 
- If the attack in progress is a Fully Charged Heavy Attack, then as an exception to the rule above, any requested attack is ignored until the completion of the attack in progress.
- If the initial movement of the queued attack is not towards the target, then instead of halting at the Resting Angle and reaccelerating, a smoother movement must be applied.

**A Combo attack** is a contiguous sequence of attacks that match the predefined sequence. For a combo to be counted, all elements of it (which are instances of attacks) except the first one need to be queued (not accepted immediately). - A Fully Charged Heavy Attack is not considered as a Heavy Attack in the context of combos. 
- A combo is not counted if it overlaps with a combo that has been counted. 
- When a combo is counted, its value is added to the combo counter.
- When a combo is detected, at the time the last attack of the combo hits the target, a triple short beep is played.
- The LEDs indicate the current value of the combo counter. [Thermometer coding](https://en.wikipedia.org/wiki/Unary_coding) is used. The counter is reset to 0 after a Special Attack is used.

## Checkpoints
The task is split into several checkpoints. To get the points of a checkpoint, you need to satisfy the requirements of both that checkpoint and all checkpoints above it. Those earlier checkpoints should be adapted to the current checkpoint. For example, when you first implement queueing it might not involve Heavy Attacks, but for Heavy Attacks checkpoint to be considered done, you also need to ensure queueing works with Heavy Attacks.

* [3] Having the weapon oscillate from side to side based on button clicks
* [2] When the weapon hits the target, a hit sound is played (short beep).
* [5] Light Attack is implemented with or without queueing.
* [12] Queueing is implemented.
* [3] When an attack request is immediately accepted or queued, a success sound is played (short beep). No special handling of overlapping sounds is required.
* [10] Heavy Attack is implemented, transition to a queued Heavy Attack is not necessarily as desired. If it becomes a Fully Charged Heavy Attack, then notify the player with a double short beep.
* [5] Transitions to a queued Heavy Attack is as desired.
* [15] These combos below are detected. LEDs show the counter value. Triple beep sound is played when the last attack of a combo hits the target.
	* Light, Light, Light, Light. (value: 0)
	* Light, Light, Light, Heavy (value: 1)
	* Heavy, Heavy, Heavy (value: 0)
	* Heavy, Heavy, Light, Heavy (value: 1)
	
	Examples regarding the combos: (Assume each example is a queued "chain")
	
	* HH**LLLH**HLH: total value is 1, only counted combo is LLLH, the HHL just after it overlaps so it is not counted.
	* **LLLL**LL**HHH**: total value is 0, counted combos are LLLL and HHH, the LLLH sandwiched inbetween overlaps and thus does not count.
* [5] Special Attack is implemented. Movement is up to your imagination. Has to look good and be impressive. A continuous beep is played during the attack,

## Video Illustration
[Here's a video](PWM%20Homework%20Illustration.avi) - Click "View raw"

The attacks in the video (in order): [Light, Light, Light, Heavy], [Heavy], [Fully Charged Heavy]

## Changelog:
* Nothing yet

## FAQ
* Nothing yet
