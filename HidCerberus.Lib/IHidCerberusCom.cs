using System.Runtime.InteropServices;

namespace HidCerberus.Lib
{
    [Guid("1B67D0E0-6C1A-4DD3-ADF8-793348ADC62A")]
    public interface IHidCerberusCom
    {
        void HidGuardianOpen();

        void HidGuardianClose();
    }
}
