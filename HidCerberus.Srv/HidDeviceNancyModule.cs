using HidSharp;
using Nancy;

namespace HidCerberus.Srv
{
    public class HidDeviceNancyModule : NancyModule
    {
        public HidDeviceNancyModule()
        {
            Get["/v1/hid/devices/get"] = _ =>
            {
                var dl = new HidDeviceLoader();
                
                return Response.AsJson(dl.GetDevices());
            };
        }
    }
}
