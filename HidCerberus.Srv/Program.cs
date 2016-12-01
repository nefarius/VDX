using System.ServiceProcess;

namespace HidCerberus.Srv
{
    internal static class Program
    {
        /// <summary>
        ///     The main entry point for the application.
        /// </summary>
        private static void Main()
        {
            ServiceBase[] ServicesToRun;
            ServicesToRun = new ServiceBase[]
            {
                new HidCerberusService()
            };
            ServiceBase.Run(ServicesToRun);
        }
    }
}