# DevAccess Concept

## Objective
Provide secure device access using a multi-sensor validation stack (RFID + PIR + Ultrasonic) with clear user feedback on an OLED display and automatic session control.

## Core Principles
- **Fail-safe by default**: device remains disabled unless all conditions are met.
- **Multi-factor physical validation**: RFID + motion + proximity.
- **Session awareness**: active session is tracked and terminated on inactivity or exit conditions.
- **Clear feedback**: OLED shows state, prompts, and timers.

## State Flow
1. **IDLE**
   - Display: "Ready / Present Card"
   - Device: OFF

2. **RFID AUTH**
   - Card read, UID verified against allow list
   - If unauthorized: show "Denied" and return to IDLE

3. **PRESENCE CHECK**
   - PIR must detect motion within a short window
   - If no motion: show "No Presence" and return to IDLE

4. **PROXIMITY CHECK**
   - Ultrasonic distance must be within threshold (e.g., < 60 cm)
   - If too far: show "Move Closer" and return to IDLE

5. **SESSION ACTIVE**
   - Device ON
   - OLED shows session timer and status
   - Session ends if:
     - RFID deauth (card removed or invalid)
     - PIR idle timeout exceeded
     - Ultrasonic distance exceeds threshold for a grace period
     - Explicit timeout reached

6. **SHUTDOWN**
   - Device OFF
   - OLED shows "Session Ended"
   - Return to IDLE

## Session Tracking
- Start time recorded at activation
- Idle timer reset on PIR motion
- Distance checks run periodically
- Auto shutdown if any condition fails beyond grace interval

## Security Notes
- Use a short RFID re-auth window at session start to reduce replay risk
- Consider logging UID + session duration to serial for audit
