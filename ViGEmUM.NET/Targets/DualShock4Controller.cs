namespace Nefarius.ViGEm.Targets
{
    using ViGEm;

    public class DualShock4Controller : ViGEmTarget, IViGEmTarget
    {
        public void PlugIn()
        {
            PlugIn(VigemTargetType.DualShock4Wired);
        }
    }
}
