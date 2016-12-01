using System.Reflection;
using System.ServiceModel;
using System.ServiceProcess;
using HidCerberus.Lib;
using log4net;

namespace HidCerberus.Srv
{
    public partial class HidCerberusService : ServiceBase
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static ServiceHost _serviceHost;

        public HidCerberusService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            Log.Info("Starting HidCerberus.Srv");

            _serviceHost?.Close();

            var binding = new NetTcpBinding
            {
                Security = new NetTcpSecurity { Mode = SecurityMode.None }
            };

            _serviceHost = new ServiceHost(typeof(HidCerberusWcf), Lib.HidCerberus.WcfUrl);
            _serviceHost.AddServiceEndpoint(typeof(IHidCerberusWcf), binding, Lib.HidCerberus.WcfUrl);

            _serviceHost.Open();
        }

        protected override void OnStop()
        {
            _serviceHost?.Close();
            _serviceHost = null;
        }
    }
}