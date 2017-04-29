using Microsoft.Win32;
using Nancy;

namespace HidCerberus.Srv
{
    public class HidCerberusNancyModule : NancyModule
    {
        public HidCerberusNancyModule()
        {
            Get["/v1/hid/whitelist/add/{id}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.CreateSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsJson(new[] {"OK"});
            };

            Get["/v1/hid/whitelist/remove/{id}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.DeleteSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsJson(new[] {"OK"});
            };

            Get["/v1/hid/whitelist/get"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidWhitelistRegistryKeyBase);
                var list = wlKey?.GetSubKeyNames();
                wlKey?.Close();

                return Response.AsJson(list);
            };

            Get["/v1/hid/whitelist/purge"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidWhitelistRegistryKeyBase);

                foreach (var subKeyName in wlKey.GetSubKeyNames())
                    Registry.LocalMachine.DeleteSubKey($"{HidWhitelistRegistryKeyBase}\\{subKeyName}");

                return Response.AsJson(new[] {"OK"});
            };

            Get["/v1/hid/affected/get"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase);
                var affected = wlKey?.GetValue("AffectedDevices") as string[];
                wlKey?.Close();

                return Response.AsJson(affected);
            };
        }

        private static string HidGuardianRegistryKeyBase => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters";

        private static string HidWhitelistRegistryKeyBase
            => $"{HidGuardianRegistryKeyBase}\\Whitelist";
    }
}