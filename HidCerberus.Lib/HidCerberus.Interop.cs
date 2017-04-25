using System.Diagnostics;
using System.Runtime.InteropServices;

namespace HidCerberus.Lib
{
    public partial class HidCerberus
    {
        [ComVisible(true)]
        public void HidGuardianOpen()
        {
            ServiceChannel?.HidAddPid(Process.GetCurrentProcess().Id);
        }

        [ComVisible(true)]
        public void HidGuardianClose()
        {
            ServiceChannel?.HidRemovePid(Process.GetCurrentProcess().Id);
        }
    }
}