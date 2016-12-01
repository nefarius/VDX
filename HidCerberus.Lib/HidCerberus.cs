using System;
using System.Diagnostics;
using System.ServiceModel;

namespace HidCerberus.Lib
{
    public class HidCerberus
    {
        private static readonly IHidCerberusWcf ServiceChannel;

        public static Uri WcfUrl => new Uri("net.tcp://localhost:26762/HidCerberusService");

        public static string WhitelistRegistryKeyBase => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Params\Whitelist";

        static HidCerberus()
        {
            var address = new EndpointAddress(WcfUrl);
            var binding = new NetTcpBinding
            {
                TransferMode = TransferMode.Streamed,
                Security = new NetTcpSecurity { Mode = SecurityMode.None }
            };
            var factory = new ChannelFactory<IHidCerberusWcf>(binding, address);

            ServiceChannel = factory.CreateChannel(address);

            ServiceChannel?.AddPid(Process.GetCurrentProcess().Id);
        }

        ~HidCerberus()
        {
            ServiceChannel?.RemovePid(Process.GetCurrentProcess().Id);
        }
    }
}
