using System;
using Nefarius.ViGEm.Exceptions;

namespace Nefarius.ViGEm.Targets
{
    public class Xbox360Controller : ViGEmTarget, IViGEmTarget
    {
        /// -------------------------------------------------------------------------------------------------
        /// <summary>   Plug in. </summary>
        /// <remarks>   Benjamin "Nefarius" Höglinger, 15.06.2017. </remarks>
        /// <exception cref="VigemBusNotFoundException">
        ///     Thrown when a Vigem Bus Not Found
        ///     error condition occurs.
        /// </exception>
        /// <exception cref="VigemInvalidTargetException">
        ///     Thrown when a Vigem Invalid
        ///     Target error condition occurs.
        /// </exception>
        /// <exception cref="VigemCallbackAlreadyRegisteredException">
        ///     Thrown when a Vigem Callback
        ///     Already Registered error
        ///     condition occurs.
        /// </exception>
        /// -------------------------------------------------------------------------------------------------
        public void PlugIn()
        {
            PlugIn(VigemTargetType.Xbox360Wired);

            var error = ViGemUm.vigem_register_xusb_notification(XusbNotification, Target);

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorBusNotFound:
                    throw new VigemBusNotFoundException(
                        StringResource.VigemBusNotFoundException);
                case ViGemUm.VigemError.VigemErrorInvalidTarget:
                    throw new VigemInvalidTargetException(
                        StringResource.VigemInvalidTargetException);
                case ViGemUm.VigemError.VigemErrorCallbackAlreadyRegistered:
                    throw new VigemCallbackAlreadyRegisteredException(
                        StringResource.VigemCallbackAlreadyRegistered);
            }
        }

        /// -------------------------------------------------------------------------------------------------
        /// <summary>   Un plug. </summary>
        /// <remarks>   Benjamin "Nefarius" Höglinger, 15.06.2017. </remarks>
        /// <exception cref="VigemCallbackNotFoundException">
        ///     Thrown when a Vigem Callback Not Found
        ///     error condition occurs.
        /// </exception>
        /// -------------------------------------------------------------------------------------------------
        public new void UnPlug()
        {
            var error = ViGemUm.vigem_unregister_xusb_notification(XusbNotification);

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorCallbackNotFound:
                    throw new VigemCallbackNotFoundException(
                        StringResource.VigemCallbackNotFoundException);
            }

            base.UnPlug();
        }

        public event Xbox360FeedbackReceivedEventHandler FeedbackReceived;

        private void XusbNotification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor, byte ledNumber)
        {
            FeedbackReceived?.Invoke(this, new Xbox360FeedbackReceivedEventArgs(largeMotor, smallMotor, ledNumber));
        }
    }

    public delegate void Xbox360FeedbackReceivedEventHandler(object sender, Xbox360FeedbackReceivedEventArgs e);

    public class Xbox360FeedbackReceivedEventArgs : EventArgs
    {
        public Xbox360FeedbackReceivedEventArgs(byte largeMotor, byte smallMotor, byte ledNumber)
        {
            LargeMotor = largeMotor;
            SmallMotor = smallMotor;
            LedNumber = ledNumber;
        }

        public byte LargeMotor { get; }

        public byte SmallMotor { get; }

        public byte LedNumber { get; }
    }
}
