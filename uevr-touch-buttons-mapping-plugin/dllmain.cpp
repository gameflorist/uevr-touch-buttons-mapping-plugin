#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <memory>

#include "uevr/Plugin.hpp"

#define KEYDOWN false
#define KEYUP true

void DebugPrint(char *Format, ...);
using namespace uevr;

#define PLUGIN_LOG_ONCE(...)               \
    static bool _logged_ = false;          \
    if (!_logged_)                         \
    {                                      \
        _logged_ = true;                   \
        API::get()->log_info(__VA_ARGS__); \
    }

class RemapPlugin : public uevr::Plugin
{
public:
    const UEVR_PluginInitializeParam *m_Param;
    const UEVR_VRData *m_VR;

    bool m_OpenXr;
    bool m_OpenVr;
    RemapPlugin() = default;

    void on_initialize() override
    {

        // This shows how to get to the API functions.
        m_Param = API::get()->param();
        m_VR = m_Param->vr;

        m_OpenXr = m_VR->is_openxr();
        m_OpenVr = m_VR->is_openvr();
        API::get()->log_info("UEVR Initializing...");
        API::get()->log_info("Backends: openxr: %d, openvr: %d\n", m_OpenXr, m_OpenVr);
    }

    void send_key(WORD key, bool key_up)
    {
        INPUT input;
        ZeroMemory(&input, sizeof(INPUT));
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        if (key_up)
            input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    //*******************************************************************************************
    // This is the controller input routine. Everything happens here.
    //*******************************************************************************************
    void on_xinput_get_state(uint32_t *retval, uint32_t user_index, XINPUT_STATE *state)
    {

        if (state == NULL)
            return;
        if (!m_VR->is_using_controllers())
            return; // If not using controllers, none of this applies.

        UEVR_InputSourceHandle LeftController = m_VR->get_left_joystick_source();
        UEVR_InputSourceHandle RightController = m_VR->get_right_joystick_source();
        static bool ATouchLeftDown = false;
        static bool BTouchLeftDown = false;
        static bool ATouchRightDown = false;
        static bool BTouchRightDown = false;
        static WORD ATouchLeftKey = '1';
        static WORD BTouchLeftKey = '2';
        static WORD ATouchRightKey = '3';
        static WORD BTouchRightKey = '4';

        if (m_OpenXr == true)
        {
            UEVR_ActionHandle ATouchLeft = m_VR->get_action_handle("/actions/default/in/AButtonTouchLeft");
            UEVR_ActionHandle BTouchLeft = m_VR->get_action_handle("/actions/default/in/BButtonTouchLeft");
            UEVR_ActionHandle ATouchRight = m_VR->get_action_handle("/actions/default/in/AButtonTouchRight");
            UEVR_ActionHandle BTouchRight = m_VR->get_action_handle("/actions/default/in/BButtonTouchRight");

            // Map ATouchLeft
            if (m_VR->is_action_active_any_joystick(ATouchLeft))
            {
                if (ATouchLeftDown == false)
                {
                    send_key(ATouchLeftKey, KEYDOWN);
                    ATouchLeftDown = true;
                    m_VR->trigger_haptic_vibration(
                        0.0f,
                        0.05f,
                        1.0f,
                        1000.0f,
                        m_VR->is_action_active(ATouchLeft, RightController) ? RightController : LeftController);
                }
            }
            else if (ATouchLeftDown == true)
            {
                send_key(ATouchLeftKey, KEYUP);
                ATouchLeftDown = false;
            }

            // Map BTouchLeft
            if (m_VR->is_action_active_any_joystick(BTouchLeft))
            {
                if (BTouchLeftDown == false)
                {
                    send_key(BTouchLeftKey, KEYDOWN);
                    BTouchLeftDown = true;
                    m_VR->trigger_haptic_vibration(
                        0.0f,
                        0.05f,
                        1.0f,
                        1000.0f,
                        m_VR->is_action_active(BTouchLeft, RightController) ? RightController : LeftController);
                }
            }
            else if (BTouchLeftDown == true)
            {
                send_key(BTouchLeftKey, KEYUP);
                BTouchLeftDown = false;
            }

            // Map ATouchRight
            if (m_VR->is_action_active_any_joystick(ATouchRight))
            {
                if (ATouchRightDown == false)
                {
                    send_key(ATouchRightKey, KEYDOWN);
                    ATouchRightDown = true;
                    m_VR->trigger_haptic_vibration(
                        0.0f,
                        0.05f,
                        1.0f,
                        1000.0f,
                        m_VR->is_action_active(ATouchRight, RightController) ? RightController : LeftController);
                }
            }
            else if (ATouchRightDown == true)
            {
                send_key(ATouchRightKey, KEYUP);
                ATouchRightDown = false;
            }

            // Map BTouchRight
            if (m_VR->is_action_active_any_joystick(BTouchRight))
            {
                if (BTouchRightDown == false)
                {
                    send_key(BTouchRightKey, KEYDOWN);
                    BTouchRightDown = true;
                    m_VR->trigger_haptic_vibration(
                        0.0f,
                        0.05f,
                        1.0f,
                        1000.0f,
                        m_VR->is_action_active(BTouchRight, RightController) ? RightController : LeftController);
                }
            }
            else if (BTouchRightDown == true)
            {
                send_key(BTouchRightKey, KEYUP);
                BTouchRightDown = false;
            }
        }
    }
};
// Actually creates the plugin. Very important that this global is created.
// The fact that it's using std::unique_ptr is not important, as long as the constructor is called in some way.
std::unique_ptr<RemapPlugin> g_plugin{new RemapPlugin()};

void DebugPrint(char *Format, ...)
{
    char FormattedMessage[512];
    va_list ArgPtr = NULL;

    /* Generate the formatted debug message. */
    va_start(ArgPtr, Format);
    vsprintf(FormattedMessage, Format, ArgPtr);
    va_end(ArgPtr);
}
