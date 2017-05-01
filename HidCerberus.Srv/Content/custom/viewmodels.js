/* Publish list of available devices */
$.getJSON("/v1/hid/devices/get", function (data) {
    function HidDevicesViewModel() {
        var self = this;
        self.hidDevices = ko.observableArray(data);
    }
    ko.applyBindings(new HidDevicesViewModel());
});