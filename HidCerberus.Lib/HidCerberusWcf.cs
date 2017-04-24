using System;
using System.Reflection;
using log4net;
using Microsoft.Win32;

namespace HidCerberus.Lib
{
    public class HidCerberusWcf : IHidCerberusWcf
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        public void HidAddPid(int id)
        {
            Log.Info($"Adding PID {id} to HidGuardian white-list");
            try
            {
                Registry.LocalMachine.CreateSubKey($"{HidCerberus.HidWhitelistRegistryKeyBase}\\{id}");
            }
            catch (Exception ex)
            {
                Log.Error(ex);
            }
        }

        public void HidRemovePid(int id)
        {
            Log.Info($"Removing PID {id} from HidGuardian white-list");
            try
            {
                Registry.LocalMachine.DeleteSubKey($"{HidCerberus.HidWhitelistRegistryKeyBase}\\{id}");
            }
            catch (Exception ex)
            {
                Log.Error(ex);
            }
        }
    }
}
