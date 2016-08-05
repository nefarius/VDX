using System;

namespace Nefarius.ViGEm
{
    public class ViGEmTarget : IDisposable
    {
        private ViGemUm.VigemTarget _target;

        static ViGEmTarget()
        {
            ViGemUm.vigem_init();
        }

        protected ViGEmTarget()
        {
            ViGemUm.VIGEM_TARGET_INIT(ref _target);
        }

        public void UnPlug()
        {
            ViGemUm.vigem_target_unplug(ref _target);
        }

        protected void PlugIn(VigemTargetType type)
        {
            ViGemUm.vigem_target_plugin(type, ref _target);

            switch (type)
            {
                case VigemTargetType.Xbox360Wired:
                    ViGemUm.vigem_register_xusb_notification(XusbNotification, _target);
                    break;
                case VigemTargetType.DualShock4Wired:
                    ViGemUm.vigem_register_ds4_notification(Ds4Notification, _target);
                    break;
                case VigemTargetType.XboxOneWired:
                    throw new NotImplementedException();
                default:
                    throw new NotImplementedException();
            }
        }

        private void Ds4Notification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor, ViGemUm.Ds4LightbarColor lightbarColor)
        {

        }

        private void XusbNotification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor, byte ledNumber)
        {

        }

        #region IDisposable Support
        private bool _disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects).
                    ViGemUm.vigem_target_unplug(ref _target);
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.

                _disposedValue = true;
            }
        }

        // TODO: override a finalizer only if Dispose(bool disposing) above has code to free unmanaged resources.
        // ~ViGEmTarget() {
        //   // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
        //   Dispose(false);
        // }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            // TODO: uncomment the following line if the finalizer is overridden above.
            // GC.SuppressFinalize(this);
        }

        #endregion
    }
}
