using System.Linq;
using System.Text.RegularExpressions;
using HidSharp;
using Nancy;

namespace HidCerberus.Srv
{
    internal class HidDeviceRefined
    {
        public string Manufacturer { get; set; }

        public string ProductName { get; set; }

        public string DevicePath { get; set; }

        public string HardwareId { get; set; }
    }

    internal static class HidDeviceExtensions
    {
        public static string GetHardwareId(this HidDevice device)
        {
            var dp = device.DevicePath;
            var usbHwIdRegex = new Regex(@"\\{2}\?\\(hid)#(vid_[a-z0-9]{4}&pid_[a-z0-9]{4}[^#]*)");
            var bthHwIdRegex =
                new Regex(
                    @"\\{2}\?\\(hid)#([{(]?[0-9A-Fa-z]{8}[-]?([0-9A-Fa-z]{4}[-]?){3}[0-9A-Fa-z]{12}[)}]?_vid&[a-z0-9]{8}_pid&[^#]*)");

            if (usbHwIdRegex.IsMatch(dp))
            {
                return $"{usbHwIdRegex.Match(dp).Groups[1].Value}\\{usbHwIdRegex.Match(dp).Groups[2].Value}".ToUpper();
            }

            if (bthHwIdRegex.IsMatch(dp))
            {
                return $"{bthHwIdRegex.Match(dp).Groups[1].Value}\\{bthHwIdRegex.Match(dp).Groups[2].Value}".ToUpper();
            }

            return string.Empty;
        }
    }

    public class HidDeviceNancyModule : NancyModule
    {
        public HidDeviceNancyModule()
        {
            Get["/v1/hid/devices/get"] = _ =>
            {
                var dl = from device in new HidDeviceLoader().GetDevices()
                         where !string.IsNullOrEmpty(device.GetHardwareId())
                         select new HidDeviceRefined
                         {
                             DevicePath = device.DevicePath,
                             Manufacturer = device.Manufacturer.Trim(),
                             ProductName = device.ProductName.Trim(),
                             HardwareId = device.GetHardwareId()
                         };

                return Response.AsJson(dl);
            };
        }
    }
}