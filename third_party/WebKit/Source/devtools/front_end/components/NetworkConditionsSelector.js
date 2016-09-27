// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @constructor
 * @param {!HTMLSelectElement} selectElement
 */
WebInspector.NetworkConditionsSelector = function(selectElement)
{
    this._selectElement = selectElement;
    this._selectElement.addEventListener("change", this._optionSelected.bind(this), false);
    this._setting = WebInspector.moduleSetting("networkConditions");
    this._setting.addChangeListener(this._settingChanged, this);
    this._populateOptions();
    this._settingChanged();
}

/** @type {!Array.<!{title: string, value: !WebInspector.NetworkManager.Conditions}>} */
WebInspector.NetworkConditionsSelector._networkConditionsPresets = [
    {title: "Offline", value: {throughput: 0 * 1024 / 8, latency: 0}},
    {title: "GPRS", value: {throughput: 50 * 1024 / 8, latency: 500}},
    {title: "Regular 2G", value: {throughput: 250 * 1024 / 8, latency: 300}},
    {title: "Good 2G", value: {throughput: 450 * 1024 / 8, latency: 150}},
    {title: "Regular 3G", value: {throughput: 750 * 1024 / 8, latency: 100}},
    {title: "Good 3G", value: {throughput: 1.5 * 1024 * 1024 / 8, latency: 40}},
    {title: "Regular 4G", value: {throughput: 4 * 1024 * 1024 / 8, latency: 20}},
    {title: "DSL", value: {throughput: 2 * 1024 * 1024 / 8, latency: 5}},
    {title: "WiFi", value: {throughput: 30 * 1024 * 1024 / 8, latency: 2}},
    {title: "No throttling", value: {throughput: -1, latency: 0}}
];

WebInspector.NetworkConditionsSelector.prototype = {
    _populateOptions: function()
    {
        var presets = WebInspector.NetworkConditionsSelector._networkConditionsPresets;
        for (var i = 0; i < presets.length; ++i) {
            var preset = presets[i];
            var throughputInKbps = preset.value.throughput / (1024 / 8);
            var isThrottling = (throughputInKbps > 0) || preset.value.latency;
            var option;
            if (!isThrottling) {
                option = new Option(preset.title, preset.title);
            } else {
                var throughputText = (throughputInKbps < 1024) ? WebInspector.UIString("%d Kbps", throughputInKbps) : WebInspector.UIString("%d Mbps", (throughputInKbps / 1024) | 0);
                var title = WebInspector.UIString("%s (%s %dms RTT)", preset.title, throughputText, preset.value.latency);
                option = new Option(title, preset.title);
                option.title = WebInspector.UIString("Maximum download throughput: %s.\r\nMinimum round-trip time: %dms.", throughputText, preset.value.latency);
            }
            option.settingValue = preset.value;
            this._selectElement.appendChild(option);
        }
    },

    _optionSelected: function()
    {
        this._setting.removeChangeListener(this._settingChanged, this);
        this._setting.set(this._selectElement.options[this._selectElement.selectedIndex].settingValue);
        this._setting.addChangeListener(this._settingChanged, this);
    },

    _settingChanged: function()
    {
        var value = this._setting.get();
        var options = this._selectElement.options;
        for (var index = 0; index < options.length; ++index) {
            var option = options[index];
            if (option.settingValue.throughput === value.throughput && option.settingValue.latency === value.latency)
                this._selectElement.selectedIndex = index;
        }
    }
}
