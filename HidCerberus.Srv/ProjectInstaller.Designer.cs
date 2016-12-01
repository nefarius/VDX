namespace HidCerberus.Srv
{
    partial class ProjectInstaller
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.serviceProcessInstallerCerberus = new System.ServiceProcess.ServiceProcessInstaller();
            this.serviceInstallerCerberus = new System.ServiceProcess.ServiceInstaller();
            // 
            // serviceProcessInstallerCerberus
            // 
            this.serviceProcessInstallerCerberus.Account = System.ServiceProcess.ServiceAccount.LocalSystem;
            this.serviceProcessInstallerCerberus.Password = null;
            this.serviceProcessInstallerCerberus.Username = null;
            // 
            // serviceInstallerCerberus
            // 
            this.serviceInstallerCerberus.Description = "Enables user-mode applications to white-list their process ID so they can access " +
    "HID devices hidden by HidGuardian.sys";
            this.serviceInstallerCerberus.DisplayName = "HidGuardian Whitelisting Service";
            this.serviceInstallerCerberus.ServiceName = "HidCerberusService";
            this.serviceInstallerCerberus.StartType = System.ServiceProcess.ServiceStartMode.Automatic;
            // 
            // ProjectInstaller
            // 
            this.Installers.AddRange(new System.Configuration.Install.Installer[] {
            this.serviceProcessInstallerCerberus,
            this.serviceInstallerCerberus});

        }

        #endregion

        private System.ServiceProcess.ServiceProcessInstaller serviceProcessInstallerCerberus;
        private System.ServiceProcess.ServiceInstaller serviceInstallerCerberus;
    }
}