using System;
using Nefarius.ViGEm.Exceptions;
using Nefarius.ViGEm.Targets.DualShock4;

namespace Nefarius.ViGEm.Targets
{
    public class DualShock4Controller : ViGEmTarget, IViGEmTarget
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
            PlugIn(VigemTargetType.DualShock4Wired);

            var error = ViGemUm.vigem_register_ds4_notification(Ds4Notification, Target);

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
            var error = ViGemUm.vigem_unregister_ds4_notification(Ds4Notification);

            // ReSharper disable once SwitchStatementMissingSomeCases
            switch (error)
            {
                case ViGemUm.VigemError.VigemErrorCallbackNotFound:
                    throw new VigemCallbackNotFoundException(
                        StringResource.VigemCallbackNotFoundException);
            }

            base.UnPlug();
        }

        public event DualShock4FeedbackReceivedEventHandler FeedbackReceived;

        private void Ds4Notification(ViGemUm.VigemTarget target, byte largeMotor, byte smallMotor,
            ViGemUm.Ds4LightbarColor lightbarColor)
        {
            var color = new LightbarColor(lightbarColor.Red, lightbarColor.Green, lightbarColor.Blue);
            FeedbackReceived?.Invoke(this, new DualShock4FeedbackReceivedEventArgs(largeMotor, smallMotor, color));
        }
    }

    public delegate void DualShock4FeedbackReceivedEventHandler(object sender, DualShock4FeedbackReceivedEventArgs e);
}
