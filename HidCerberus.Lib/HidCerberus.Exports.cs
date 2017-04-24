using System.Diagnostics;
using System.Runtime.InteropServices;
using RGiesecke.DllExport;

namespace HidCerberus.Lib
{
    public partial class HidCerberus
    {
        #region DLL Exports

        [DllExport(CallingConvention = CallingConvention.StdCall)]
        public static void HidGuardianOpen()
        {
            ServiceChannel?.HidAddPid(Process.GetCurrentProcess().Id);
        }

        [DllExport(CallingConvention = CallingConvention.StdCall)]
        public static void HidGuardianClose()
        {
            ServiceChannel?.HidRemovePid(Process.GetCurrentProcess().Id);
        }

        #endregion
    }
}