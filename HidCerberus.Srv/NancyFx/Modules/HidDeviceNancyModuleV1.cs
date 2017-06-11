using System.Linq;
using System.Text.RegularExpressions;
using HidSharp;
using Nancy;

namespace HidCerberus.Srv.NancyFx.Modules
{
    internal static class HidDeviceExtensions
    {
        public static string GetHardwareId(this HidDevice device)
        {
            var dp = device.DevicePath;
            var regexes = new[]
            {
                // USB notation
                new Regex(@"\\{2}\?\\(hid)#(vid_[a-z0-9]{4}&pid_[a-z0-9]{4}[^#]*)"),
                // Bluetooth service notation
                new Regex(
                    @"\\{2}\?\\(hid)#([{(]?[0-9A-Fa-z]{8}[-]?([0-9A-Fa-z]{4}[-]?){3}[0-9A-Fa-z]{12}[)}]?_vid&[a-z0-9]{8}_pid&[^#]*)")
            };

            foreach (var regex in regexes)
            {
                if (regex.IsMatch(dp)) return $"{regex.Match(dp).Groups[1].Value}\\{regex.Match(dp).Groups[2].Value}".ToUpper();
            }

            return string.Empty;
        }
    }

    public class HidDeviceNancyModuleV1 : NancyModule
    {
        public HidDeviceNancyModuleV1() : base("/api/v1")
        {
            Get["/hid/devices/get"] = _ =>
            {
                var dl = from device in new HidDeviceLoader().GetDevices()
                         where !string.IsNullOrEmpty(device.GetHardwareId())
                         select new
                         {
                             Manufacturer = device.Manufacturer.Trim(),
                             ProductName = device.ProductName.Trim(),
                             device.DevicePath,
                             HardwareId = device.GetHardwareId()
                         };

                return Response.AsJson(dl);
            };
        }
    }
}