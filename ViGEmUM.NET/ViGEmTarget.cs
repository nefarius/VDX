using System;
using Nefarius.ViGEm.Exceptions;

namespace Nefarius.ViGEm
{
    ///-------------------------------------------------------------------------------------------------
    /// <summary>   A ViGEm target. </summary>
    ///
    /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    public partial class ViGEmTarget : IDisposable
    {
        private ViGemUm.VigemTarget _target;

        ///-------------------------------------------------------------------------------------------------
        /// <summary>   Static constructor. </summary>
        ///
        /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
        ///
        /// <exception cref="VigemBusNotFoundException">        Thrown when a Vigem Bus Not Found error
        ///                                                     condition occurs.
        /// </exception>
        /// <exception cref="VigemBusAccessFailedException">    Thrown when a Vigem Bus Access Failed
        ///                                                     error condition occurs.
        /// </exception>
        /// <exception cref="VigemBusVersionMismatchException"> Thrown when a Vigem Bus Version Mismatch
        ///                                                     error condition occurs.
        /// </exception>
        ///-------------------------------------------------------------------------------------------------
        static ViGEmTarget()
        {
            var error = ViGemUm.vigem_init();

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorNone:
                    return;
                case ViGemUm.VigemError.VigemErrorBusNotFound:
                    throw new VigemBusNotFoundException(
                        StringResource.VigemBusNotFoundException);
                case ViGemUm.VigemError.VigemErrorBusAccessFailed:
                    throw new VigemBusAccessFailedException(
                        "Access error on opening the Virtual Gamepad Emulation Bus.");
                case ViGemUm.VigemError.VigemErrorBusVersionMismatch:
                    throw new VigemBusVersionMismatchException(
                        "The library version doesn't match the reported bus version. Please check if there's a software update available.");
            }
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>   Specialised default constructor for use only by derived class. </summary>
        ///
        /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
        ///-------------------------------------------------------------------------------------------------
        protected ViGEmTarget()
        {
            ViGemUm.VIGEM_TARGET_INIT(ref _target);
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>   Unplug target device. </summary>
        ///
        /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
        ///
        /// <exception cref="VigemBusNotFoundException">            Thrown when a Vigem Bus Not Found
        ///                                                         error condition occurs.
        /// </exception>
        /// <exception cref="VigemTargetUninitializedException">    Thrown when a Vigem Target
        ///                                                         Uninitialized error condition occurs.
        /// </exception>
        /// <exception cref="VigemTargetNotPluggedInException">     Thrown when a Vigem Target Not
        ///                                                         Plugged In error condition occurs.
        /// </exception>
        /// <exception cref="VigemRemovalFailedException">          Thrown when a Vigem Removal Failed
        ///                                                         error condition occurs.
        /// </exception>
        ///-------------------------------------------------------------------------------------------------
        public void UnPlug()
        {
            var error = ViGemUm.vigem_target_unplug(ref _target);

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorNone:
                    break;
                case ViGemUm.VigemError.VigemErrorBusNotFound:
                    throw new VigemBusNotFoundException(
                        StringResource.VigemBusNotFoundException);
                case ViGemUm.VigemError.VigemErrorTargetUninitialized:
                    throw new VigemTargetUninitializedException(
                        StringResource.VigemTargetUninitializedException);
                case ViGemUm.VigemError.VigemErrorTargetNotPluggedIn:
                    throw new VigemTargetNotPluggedInException(
                        StringResource.VigemTargetNotPluggedInException);
                case ViGemUm.VigemError.VigemErrorRemovalFailed:
                    throw new VigemRemovalFailedException(
                        StringResource.VigemRemovalFailedException);
            }
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>   Plug in target device. </summary>
        ///
        /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
        ///
        /// <exception cref="VigemBusNotFoundException">            Thrown when a Vigem Bus Not Found
        ///                                                         error condition occurs.
        /// </exception>
        /// <exception cref="VigemTargetUninitializedException">    Thrown when a Vigem Target
        ///                                                         Uninitialized error condition occurs.
        /// </exception>
        /// <exception cref="VigemAlreadyConnectedException">       Thrown when a Vigem Already Connected
        ///                                                         error condition occurs.
        /// </exception>
        /// <exception cref="VigemNoFreeSlotException">             Thrown when a Vigem No Free Slot
        ///                                                         error condition occurs.
        /// </exception>
        ///
        /// <param name="type"> The type. </param>
        ///-------------------------------------------------------------------------------------------------
        protected void PlugIn(VigemTargetType type)
        {
            var error = ViGemUm.vigem_target_plugin(type, ref _target);

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorNone:
                    break;
                case ViGemUm.VigemError.VigemErrorBusNotFound:
                    throw new VigemBusNotFoundException(
                        StringResource.VigemBusNotFoundException);
                case ViGemUm.VigemError.VigemErrorTargetUninitialized:
                    throw new VigemTargetUninitializedException(
                        StringResource.VigemTargetUninitializedException);
                case ViGemUm.VigemError.VigemErrorAlreadyConnected:
                    throw new VigemAlreadyConnectedException(
                        StringResource.VigemAlreadyConnectedException);
                case ViGemUm.VigemError.VigemErrorNoFreeSlot:
                    throw new VigemNoFreeSlotException(
                        StringResource.VigemNoFreeSlotException);
            }

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

        private void Ds4Notification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor,
            ViGemUm.Ds4LightbarColor lightbarColor)
        {
        }

        private void XusbNotification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor, byte ledNumber)
        {
        }

        #region IDisposable Support

        private bool _disposedValue; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                    ViGemUm.vigem_target_unplug(ref _target);

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

        public enum VigemTargetType
        {
            Xbox360Wired,
            XboxOneWired,
            DualShock4Wired
        }
    }
}
