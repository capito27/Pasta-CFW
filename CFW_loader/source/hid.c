#include "hid.h"

// @breif  Wait for user input.
// @retval the button pressed. see BUTTON_* macros in hid.h.
// @note   This would wait until a key is up or down.
u32 HidWaitForInput(void) {
    u32 pad_state_old = HID_STATE;
    while (true) {
        u32 pad_state = HID_STATE;
        if (pad_state ^ pad_state_old)
            return ~pad_state;
    }
}
