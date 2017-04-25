using System;
using System.Reflection;
using System.ServiceModel;
using log4net;
using System.Runtime.InteropServices;

namespace HidCerberus.Lib
{
    [ComVisible(true)]
    [ClassInterface(ClassInterfaceType.None)]
    [Guid("9C4B3DE9-C1C7-44AF-AAF7-10E18162CACA")]
    public partial class HidCerberus : IHidCerberusCom
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

        public static string HidWhitelistRegistryKeyBase
            => @"SYSTEM\CurrentControlSet\Services\HidGuardian\Parameters\Whitelist";

        public static string XnaWhitelistRegistryKeyBase
            => @"SYSTEM\CurrentControlSet\Services\XnaGuardian\Parameters\Whitelist";

        #endregion
    }
}