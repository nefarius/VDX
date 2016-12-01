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
            var address = new EndpointAddress(WcfUrl);
            var binding = new NetTcpBinding
            {
                Security = new NetTcpSecurity {Mode = SecurityMode.None}
            };
            var factory = new ChannelFactory<IHidCerberusWcf>(binding, address);

            try
            {
                ServiceChannel = factory.CreateChannel(address);
            }
            catch (Exception ex)
            {
                Log.Error(ex);
            }
        }

        #region Static Properties

        public static Uri WcfUrl => new Uri("net.tcp://localhost:26762/HidCerberusService");

        public static string WhitelistRegistryKeyBase
            => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters\Whitelist";

        #endregion

        #region DLL Exports

        [DllExport(CallingConvention = CallingConvention.StdCall)]
        public static void HidGuardianOpen()
        {
            ServiceChannel?.AddPid(Process.GetCurrentProcess().Id);
        }

        [DllExport(CallingConvention = CallingConvention.StdCall)]
        public static void HidGuardianClose()
        {
            ServiceChannel?.RemovePid(Process.GetCurrentProcess().Id);
        }

        #endregion
    }
}