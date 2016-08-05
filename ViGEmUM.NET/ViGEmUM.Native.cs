using System.Runtime.InteropServices;

namespace Nefarius.ViGEm
{
    public partial class ViGEmUM
    {
        #region Enums

        private enum VIGEM_ERROR
        {
            VIGEM_ERROR_NONE = 0x0000,
            VIGEM_ERROR_BUS_NOT_FOUND,
            VIGEM_ERROR_NO_FREE_SLOT,
            VIGEM_ERROR_INVALID_TARGET,
            VIGEM_ERROR_REMOVAL_FAILED,
            VIGEM_ERROR_ALREADY_CONNECTED,
            VIGEM_ERROR_TARGET_UNINITIALIZED,
            VIGEM_ERROR_TARGET_NOT_PLUGGED_IN
        }

        private enum VIGEM_TARGET_TYPE
        {
            Xbox360Wired,
            XboxOneWired,
            DualShock4Wired
        }

        private enum VIGEM_TARGET_STATE
        {
            VigemTargetNew,
            VigemTargetInitialized,
            VigemTargetConnected,
            VigemTargetDisconnected
        }

        #endregion

        #region Structs

        [StructLayout(LayoutKind.Sequential)]
        private struct VIGEM_TARGET
        {
            ulong Size;
            ushort Version;
            ulong SerialNo;
            VIGEM_TARGET_STATE State;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct DS4_LIGHTBAR_COLOR
        {
            byte Red;
            byte Green;
            byte Blue;
        };

        [StructLayout(LayoutKind.Sequential)]
        private struct XUSB_REPORT
        {
            ushort wButtons;
            byte bLeftTrigger;
            byte bRightTrigger;
            short sThumbLX;
            short sThumbLY;
            short sThumbRX;
            short sThumbRY;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct DS4_REPORT
        {
            byte bThumbLX;
            byte bThumbLY;
            byte bThumbRX;
            byte bThumbRY;
            ushort wButtons;
            byte bSpecial;
            byte bTriggerL;
            byte bTriggerR;
        }

        #endregion

        private delegate void VIGEM_XUSB_NOTIFICATION(
            VIGEM_TARGET Target,
            byte LargeMotor,
            byte SmallMotor,
            byte LedNumber);

        private delegate void VIGEM_DS4_NOTIFICATION(
            VIGEM_TARGET Target,
            byte LargeMotor,
            byte SmallMotor,
            DS4_LIGHTBAR_COLOR LightbarColor);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_init();

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern void vigem_shutdown();

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_target_plugin(
            [In] VIGEM_TARGET_TYPE Type,
            [In, Out] ref VIGEM_TARGET Target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern void vigem_target_unplug(
            [In, Out] ref VIGEM_TARGET Target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_register_xusb_notification(
            [In, MarshalAs(UnmanagedType.FunctionPtr)] VIGEM_XUSB_NOTIFICATION Notification,
            [In] VIGEM_TARGET Target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_register_ds4_notification(
            [In, MarshalAs(UnmanagedType.FunctionPtr)] VIGEM_DS4_NOTIFICATION Notification,
            [In] VIGEM_TARGET Target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_xusb_submit_report(
            [In] VIGEM_TARGET Target,
            [In] XUSB_REPORT Report);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern VIGEM_ERROR vigem_ds4_submit_report(
            [In] VIGEM_TARGET Target,
            [In] DS4_REPORT Report);
    }
}
