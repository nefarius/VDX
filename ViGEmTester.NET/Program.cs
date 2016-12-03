using System;
using Nefarius.ViGEm.Targets;

namespace ViGEmTester.NET
{
    class Program
    {
        static void Main(string[] args)
        {
            HidCerberus.Lib.HidCerberus.HidGuardianOpen();

            var x360 = new Xbox360Controller();
            x360.PlugIn();

            Console.ReadKey();
            
            var ds4 = new DualShock4Controller();
            ds4.PlugIn();

            Console.ReadKey();

            HidCerberus.Lib.HidCerberus.HidGuardianClose();
        }
    }
}
