using Nancy;

namespace HidCerberus.Srv.NancyFx
{
    public class IndexModule : NancyModule
    {
        public IndexModule()
        {
            Get["/"] = _ => View["index"];
        }
    }
}
