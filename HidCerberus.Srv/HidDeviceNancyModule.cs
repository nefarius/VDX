using HidSharp;
using Nancy;

namespace HidCerberus.Srv
{
    public class HidDeviceNancyModule : NancyModule
    {
        public HidDeviceNancyModule()
        {
            Get["/v1/hid/devices/get"] = _ => Response.AsJson(new HidDeviceLoader().GetDevices());
        }
    }
}
