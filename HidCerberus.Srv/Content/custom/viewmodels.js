/* Publish list of available devices */
$.getJSON("/v1/hid/devices/get", function (data) {
    function HidDevicesViewModel() {
        var self = this;
        self.hidDevices = ko.observableArray(data);

        self.hideDevice = function (device) {
            $.post("/v1/hidguardian/affected/add", { hwids: device.hardwareId }, function(data, status) {
                $("#submitted-hide-dialog").modal();
            });
        }
    }
    ko.applyBindings(new HidDevicesViewModel());
});
