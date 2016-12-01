using System.ServiceModel;

namespace HidCerberus.Lib
{
    [ServiceContract]
    public interface IHidCerberusWcf
    {
        [OperationContract]
        void AddPid(int id);

        [OperationContract]
        void RemovePid(int id);
    }
}