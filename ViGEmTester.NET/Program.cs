using System;
using Nefarius.ViGEm.Targets;

namespace ViGEmTester.NET
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var x360 = new Xbox360Controller();
            x360.FeedbackReceived +=
                (sender, eventArgs) => Console.WriteLine(
                    $"LM: {eventArgs.LargeMotor}, " +
                    $"SM: {eventArgs.SmallMotor}, " +
                    $"LED: {eventArgs.LedNumber}");
            x360.PlugIn();

            Console.ReadKey();

            var ds4 = new DualShock4Controller();
            ds4.FeedbackReceived +=
                (sender, eventArgs) => Console.WriteLine(
                    $"LM: {eventArgs.LargeMotor}, " +
                    $"SM: {eventArgs.SmallMotor}, ");
            ds4.PlugIn();

            Console.ReadKey();
        }
    }
}