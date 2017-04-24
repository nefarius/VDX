using System.ServiceModel;

namespace HidCerberus.Lib
{
    [ServiceContract]
    public interface IHidCerberusWcf
    {
        [OperationContract]
        void HidAddPid(int id);

        [OperationContract]
        void HidRemovePid(int id);
    }
}