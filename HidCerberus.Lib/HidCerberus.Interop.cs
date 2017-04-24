using System.Diagnostics;
using System.Runtime.InteropServices;

namespace HidCerberus.Lib
{
    public partial class HidCerberus
    {
        #region DLL Exports

        public void HidGuardianOpen()
        {
            ServiceChannel?.HidAddPid(Process.GetCurrentProcess().Id);
        }

        public void HidGuardianClose()
        {
            ServiceChannel?.HidRemovePid(Process.GetCurrentProcess().Id);
        }

        #endregion
    }
}