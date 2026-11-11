#include <psp2/ctrl.h>
#include <psp2/kernel/sysmem.h>
#include <string.h>
#include "vita_compat.h"

static s8 scale_axis(unsigned char value) {
    int centered = (int)value - 128;

    if (centered > 0) {
        return (s8)((centered * 80) / 127);
    }

    return (s8)((centered * 80) / 128);
}

static void map_ctrl_data(OSContPad *pad, const SceCtrlData *ctrl) {
    u16 buttons = 0;

    if (ctrl->buttons & SCE_CTRL_CROSS)     buttons |= CONT_A;
    if (ctrl->buttons & SCE_CTRL_CIRCLE)    buttons |= CONT_B;
    if (ctrl->buttons & SCE_CTRL_START)     buttons |= CONT_START;

    if (ctrl->buttons & SCE_CTRL_UP)        buttons |= CONT_UP;
    if (ctrl->buttons & SCE_CTRL_DOWN)      buttons |= CONT_DOWN;
    if (ctrl->buttons & SCE_CTRL_LEFT)      buttons |= CONT_LEFT;
    if (ctrl->buttons & SCE_CTRL_RIGHT)     buttons |= CONT_RIGHT;

    /* Positive2 reports wireless-pad shoulder input using L1/R1 while the
     * Vita-local API historically used LTRIGGER/RTRIGGER. Accept both so the
     * same N64 L/R actions work on handheld and DS4. */
    if (ctrl->buttons & (SCE_CTRL_LTRIGGER | SCE_CTRL_L1)) buttons |= CONT_L;
    if (ctrl->buttons & (SCE_CTRL_RTRIGGER | SCE_CTRL_R1)) buttons |= CONT_R;

    /* Temporary N64 C-button mapping retained from the single-player port. */
    if (ctrl->buttons & SCE_CTRL_TRIANGLE)  buttons |= CONT_E;
    if (ctrl->buttons & SCE_CTRL_SQUARE)    buttons |= CONT_C;

    pad->button = buttons;
    pad->stick_x = scale_axis(ctrl->lx);
    /* Vita Y axis increases downward; N64 positive Y is upward. */
    pad->stick_y = (s8)-scale_axis(ctrl->ly);
    pad->errno = 0;
}

void vita_read_pad(OSContPad *pad) {
    SceCtrlData ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.lx = ctrl.ly = ctrl.rx = ctrl.ry = 128;
    if (sceCtrlPeekBufferPositive(0, &ctrl, 1) < 0) {
        memset(pad, 0, sizeof(*pad));
        pad->errno = 1;
        return;
    }
    map_ctrl_data(pad, &ctrl);
}

void vita_read_pad_port(OSContPad *pad, int port) {
    SceCtrlData ctrl;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.lx = ctrl.ly = ctrl.rx = ctrl.ry = 128;
    if (sceCtrlPeekBufferPositive2(port, &ctrl, 1) < 0) {
        memset(pad, 0, sizeof(*pad));
        pad->errno = 1;
        return;
    }
    map_ctrl_data(pad, &ctrl);
}

int vita_is_pstv(void) {
    return sceKernelGetModel() == SCE_KERNEL_MODEL_VITATV;
}

int vita_get_external_controller_info(int *ports, int *types, int max_ports) {
    SceCtrlPortInfo info;
    int count = 0;
    int port;

    if (!vita_is_pstv() || max_ports <= 0)
        return 0;

    memset(&info, 0, sizeof(info));
    if (sceCtrlGetControllerPortInfo(&info) < 0)
        return 0;

    /* On PSTV, paired wireless pads are reported as DS3/DS4 controller
     * types. Do not count the system's virtual port as a separate player. */
    for (port = 0; port < 5; port++) {
        if (info.port[port] == SCE_CTRL_TYPE_DS3 ||
            info.port[port] == SCE_CTRL_TYPE_DS4) {
            if (count < max_ports) {
                if (ports != NULL)
                    ports[count] = port;
                if (types != NULL)
                    types[count] = info.port[port];
            }
            count++;
        }
    }
    return count;
}

int vita_get_external_controller_ports(int *ports, int max_ports) {
    return vita_get_external_controller_info(ports, NULL, max_ports);
}
