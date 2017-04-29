using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;
using Nancy;

namespace HidCerberus.Srv
{
    public class HidCerberusNancyModule : NancyModule
    {
        private static readonly IEnumerable<object> ResponseOk = new[] {"OK"};
        private static readonly string[] HardwareIdSplitters = {"\r\n", "\n"};

        public HidCerberusNancyModule()
        {
            #region Whitelist

            Get["/v1/hidguardian/whitelist/add/{id:int}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.CreateSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsJson(ResponseOk);
            };

            Get["/v1/hidguardian/whitelist/remove/{id:int}"] = parameters =>
            {
                var id = int.Parse(parameters.id);

                Registry.LocalMachine.DeleteSubKey($"{HidWhitelistRegistryKeyBase}\\{id}");

                return Response.AsJson(ResponseOk);
            };

            Get["/v1/hidguardian/whitelist/get"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidWhitelistRegistryKeyBase);
                var list = wlKey?.GetSubKeyNames();
                wlKey?.Close();

                return Response.AsJson(list);
            };

            Get["/v1/hidguardian/whitelist/purge"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidWhitelistRegistryKeyBase);

                foreach (var subKeyName in wlKey.GetSubKeyNames())
                    Registry.LocalMachine.DeleteSubKey($"{HidWhitelistRegistryKeyBase}\\{subKeyName}");

                return Response.AsJson(ResponseOk);
            };

            #endregion

            #region Force

            Get["/v1/hidguardian/force/get"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase);
                var force = wlKey?.GetValue("Force");
                wlKey?.Close();

                return Response.AsJson(force);
            };

            Get["/v1/hidguardian/force/enable"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase, true);
                wlKey?.SetValue("Force", 1);
                wlKey?.Close();

                return Response.AsJson(ResponseOk);
            };

            Get["/v1/hidguardian/force/disable"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase, true);
                wlKey?.SetValue("Force", 0);
                wlKey?.Close();

                return Response.AsJson(ResponseOk);
            };

            #endregion

            #region Affected

            Get["/v1/hidguardian/affected/get"] = _ =>
            {
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase);
                var affected = wlKey?.GetValue("AffectedDevices") as string[];
                wlKey?.Close();

                return Response.AsJson(affected);
            };

            Get["/v1/hidguardian/affected/add/{hwid}"] = parameters =>
            {
                // decode base64 input
                var base64 = (string) parameters.hwid;
                var hwids = Encoding.UTF8.GetString(Convert.FromBase64String(base64));

                // get existing Hardware IDs
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase, true);
                var affected = (wlKey?.GetValue("AffectedDevices") as string[])?.ToList();

                // split input array
                var idList = hwids.Split(HardwareIdSplitters, StringSplitOptions.None).ToList();

                // fuse arrays
                if (affected != null)
                    idList.AddRange(affected);

                // write back to registry
                wlKey?.SetValue("AffectedDevices", idList.Where(s => !string.IsNullOrWhiteSpace(s)).Distinct().ToArray(),
                    RegistryValueKind.MultiString);

                wlKey?.Close();

                return Response.AsJson(ResponseOk);
            };

            Get["/v1/hidguardian/affected/remove/{hwid}"] = parameters =>
            {
                // decode base64 input
                var base64 = (string) parameters.hwid;
                var hwids = Encoding.UTF8.GetString(Convert.FromBase64String(base64));

                // get existing Hardware IDs
                var wlKey = Registry.LocalMachine.OpenSubKey(HidGuardianRegistryKeyBase, true);
                var affected = (wlKey?.GetValue("AffectedDevices") as string[])?.ToList();

                // split input array
                var idList = hwids.Split(HardwareIdSplitters, StringSplitOptions.None).ToList();

                // remove provided values
                affected?.RemoveAll(x => idList.Contains(x));

                // write back to registry
                wlKey?.SetValue("AffectedDevices", idList.Where(s => !string.IsNullOrWhiteSpace(s)).Distinct().ToArray(),
                    RegistryValueKind.MultiString);

                wlKey?.Close();

                return Response.AsJson(ResponseOk);
            };

            #endregion
        }

        private static string HidGuardianRegistryKeyBase => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters";

        private static string HidWhitelistRegistryKeyBase
            => $"{HidGuardianRegistryKeyBase}\\Whitelist";
    }
}