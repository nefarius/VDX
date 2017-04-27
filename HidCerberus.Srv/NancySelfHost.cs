using HidCerberus.Srv.Properties;
using Nancy.Hosting.Self;

namespace HidCerberus.Srv
{
    public class NancySelfHost
    {
        private NancyHost _nancyHost;

        public void Start()
        {
            _nancyHost = new NancyHost(Settings.Default.ServiceUrl);
            _nancyHost.Start();
        }

        public void Stop()
        {
            _nancyHost.Stop();
        }
    }
}
