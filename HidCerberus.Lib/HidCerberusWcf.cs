using Microsoft.Win32;

namespace HidCerberus.Lib
{
    public class HidCerberusWcf : IHidCerberusWcf
    {
        public void AddPid(int id)
        {
            Registry.LocalMachine.CreateSubKey($"{HidCerberus.WhitelistRegistryKeyBase}\\{id}");
        }

        public void RemovePid(int id)
        {
            Registry.LocalMachine.DeleteSubKey($"{HidCerberus.WhitelistRegistryKeyBase}\\{id}");
        }
    }
}