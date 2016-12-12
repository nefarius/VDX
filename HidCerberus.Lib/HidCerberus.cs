using System;
using System.Reflection;
using System.ServiceModel;
using log4net;

namespace HidCerberus.Lib
{
    public partial class HidCerberus : IDisposable
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

        #region IDisposable Support

        private bool _disposedValue; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (_disposedValue) return;

            if (disposing)
                HidGuardianClose();

            // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
            // TODO: set large fields to null.

            _disposedValue = true;
        }

        // TODO: override a finalizer only if Dispose(bool disposing) above has code to free unmanaged resources.
        // ~HidCerberus() {
        //   // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
        //   Dispose(false);
        // }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            // TODO: uncomment the following line if the finalizer is overridden above.
            // GC.SuppressFinalize(this);
        }

        #endregion

        #endregion
    }
}