using System;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.ServiceModel;
using log4net;
using RGiesecke.DllExport;

namespace HidCerberus.Lib
{
    public class HidCerberus
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly IHidCerberusWcf ServiceChannel;

        static HidCerberus()
        {
            Log.Info("Loading HidCerberus.Lib");

            AppDomain.CurrentDomain.ProcessExit += (sender, args) =>
            {
                Log.Info("Library shutdown");
                ServiceChannel?.RemovePid(Process.GetCurrentProcess().Id);
            };

            var address = new EndpointAddress(WcfUrl);
            var binding = new NetTcpBinding
            {
                Security = new NetTcpSecurity {Mode = SecurityMode.None}
            };
            var factory = new ChannelFactory<IHidCerberusWcf>(binding, address);

            try
            {
                ServiceChannel = factory.CreateChannel(address);
                ServiceChannel?.AddPid(Process.GetCurrentProcess().Id);
            }
            catch (Exception ex)
            {
                Log.Error(ex);
            }
        }

        public static Uri WcfUrl => new Uri("net.tcp://localhost:26762/HidCerberusService");

        public static string WhitelistRegistryKeyBase
            => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters\Whitelist";

        [DllExport(CallingConvention = CallingConvention.StdCall)]
        public static void Dummy()
        {
        }
    }
}