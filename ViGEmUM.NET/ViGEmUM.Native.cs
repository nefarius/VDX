using System.Runtime.InteropServices;

namespace Nefarius.ViGEm
{
    public enum VigemTargetType
    {
        Xbox360Wired,
        XboxOneWired,
        DualShock4Wired
    }

    internal static partial class ViGemUm
    {
        #region Enums

        public enum VigemError : uint
        {
            VigemErrorNone = 0x20000000,
            VigemErrorBusNotFound = 0xE0000001,
            VigemErrorNoFreeSlot = 0xE0000002,
            VigemErrorInvalidTarget = 0xE0000003,
            VigemErrorRemovalFailed = 0xE0000004,
            VigemErrorAlreadyConnected = 0xE0000005,
            VigemErrorTargetUninitialized = 0xE0000006,
            VigemErrorTargetNotPluggedIn = 0xE0000007,
            VigemErrorBusVersionMismatch = 0xE0000008,
            VigemErrorBusAccessFailed = 0xE0000009
        }

        public enum VigemTargetState
        {
            VigemTargetNew,
            VigemTargetInitialized,
            VigemTargetConnected,
            VigemTargetDisconnected
        }

        #endregion

        #region Structs

        [StructLayout(LayoutKind.Sequential)]
        public struct VigemTarget
        {
            public uint Size;
            public ushort Version;
            public uint SerialNo;
            public VigemTargetState State;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Ds4LightbarColor
        {
            public byte Red;
            public byte Green;
            public byte Blue;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct XusbReport
        {
            public ushort wButtons;
            public byte bLeftTrigger;
            public byte bRightTrigger;
            public short sThumbLX;
            public short sThumbLY;
            public short sThumbRX;
            public short sThumbRY;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Ds4Report
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

        public delegate void VigemXusbNotification(
            VigemTarget target,
            byte largeMotor,
            byte smallMotor,
            byte ledNumber);

        public delegate void VigemDs4Notification(
            VigemTarget target,
            byte largeMotor,
            byte smallMotor,
            Ds4LightbarColor lightbarColor);

        public static void VIGEM_TARGET_INIT(
            [In, Out] ref VigemTarget target)
        {
            target.Size = (uint)Marshal.SizeOf(typeof(VigemTarget));
            target.Version = 1;
            target.State = VigemTargetState.VigemTargetInitialized;
        }

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_init();

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern void vigem_shutdown();

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_target_plugin(
            [In] VigemTargetType type,
            [In, Out] ref VigemTarget target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern void vigem_target_unplug(
            [In, Out] ref VigemTarget target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_register_xusb_notification(
            [In, MarshalAs(UnmanagedType.FunctionPtr)] VigemXusbNotification notification,
            [In] VigemTarget target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_register_ds4_notification(
            [In, MarshalAs(UnmanagedType.FunctionPtr)] VigemDs4Notification notification,
            [In] VigemTarget target);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_xusb_submit_report(
            [In] VigemTarget target,
            [In] XusbReport report);

        [DllImport("ViGEmUM.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        public static extern VigemError vigem_ds4_submit_report(
            [In] VigemTarget target,
            [In] Ds4Report report);
    }
}
