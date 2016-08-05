namespace Nefarius.ViGEm.Targets
{
    using ViGEm;

    public class Xbox360Controller : ViGEmTarget, IViGEmTarget
    {
        public void PlugIn()
        {
            PlugIn(VigemTargetType.Xbox360Wired);
        }
    }
}
