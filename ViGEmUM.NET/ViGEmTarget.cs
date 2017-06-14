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
        protected ViGemUm.VigemTarget Target;

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
                        StringResource.VigemBusAccessFailedException);
                case ViGemUm.VigemError.VigemErrorBusVersionMismatch:
                    throw new VigemBusVersionMismatchException(
                        StringResource.VigemBusVersionMismatchException);
            }
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>   Specialised default constructor for use only by derived class. </summary>
        ///
        /// <remarks>   Benjamin "Nefarius" Höglinger, 12.06.2017. </remarks>
        ///-------------------------------------------------------------------------------------------------
        protected ViGEmTarget()
        {
            ViGemUm.VIGEM_TARGET_INIT(ref Target);
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
        protected void UnPlug()
        {
            var error = ViGemUm.vigem_target_unplug(ref Target);

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
            var error = ViGemUm.vigem_target_plugin(type, ref Target);

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
        }

        #region IDisposable Support

        private bool _disposedValue; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                    ViGemUm.vigem_target_unplug(ref Target);

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

        protected enum VigemTargetType
        {
            Xbox360Wired,
            XboxOneWired,
            DualShock4Wired
        }
    }
}
