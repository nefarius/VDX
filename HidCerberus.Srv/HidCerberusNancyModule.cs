using Microsoft.Win32;
using Nancy;

namespace HidCerberus.Srv
{
    public class HidCerberusNancyModule : NancyModule
    {
        private static string HidWhitelistRegistryKeyBase
            => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters\Whitelist";

        public HidCerberusNancyModule()
        {
            Get["/v1/hid/whitelist/add/{id}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.CreateSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsText("OK");
            };

            Get["/v1/hid/whitelist/remove/{id}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.DeleteSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsText("OK");
            };
        }
    }
}
