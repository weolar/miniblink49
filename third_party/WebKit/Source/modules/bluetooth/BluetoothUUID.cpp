// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/bluetooth/BluetoothUUID.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptRegexp.h"
#include "core/dom/ExceptionCode.h"
#include "platform/UUID.h"
#include "wtf/HashMap.h"
#include "wtf/HexNumber.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

typedef WTF::HashMap<String, unsigned> NameToAssignedNumberMap;

enum class GATTAttribute {
    Service,
    Characteristic,
    Descriptor
};

NameToAssignedNumberMap* getAssignedNumberToServiceNameMap()
{
    AtomicallyInitializedStaticReference(NameToAssignedNumberMap, servicesMap, []() {
        // https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
        NameToAssignedNumberMap* services = new NameToAssignedNumberMap();
        services->add("alert_notification", 0x1811);
        services->add("battery_service", 0x180F);
        services->add("blood_pressure", 0x1810);
        services->add("body_composition", 0x181B);
        services->add("bond_management", 0x181E);
        services->add("continuous_glucose_monitoring", 0x181F);
        services->add("current_time", 0x1805);
        services->add("cycling_power", 0x1818);
        services->add("cycling_speed_and_cadence", 0x1816);
        services->add("device_information", 0x180A);
        services->add("environmental_sensing", 0x181A);
        services->add("generic_access", 0x1800);
        services->add("generic_attribute", 0x1801);
        services->add("glucose", 0x1808);
        services->add("health_thermometer", 0x1809);
        services->add("heart_rate", 0x180D);
        services->add("human_interface_device", 0x1812);
        services->add("immediate_alert", 0x1802);
        services->add("indoor_positioning", 0x1821);
        services->add("internet_protocol_support", 0x1820);
        services->add("link_loss", 0x1803);
        services->add("location_and_navigation", 0x1819);
        services->add("next_dst_change", 0x1807);
        services->add("phone_alert_status", 0x180E);
        services->add("reference_time_update", 0x1806);
        services->add("running_speed_and_cadence", 0x1814);
        services->add("scan_parameters", 0x1813);
        services->add("tx_power", 0x1804);
        services->add("user_data", 0x181C);
        services->add("weight_scale", 0x181D);
        return services;
    }());

    return &servicesMap;
}

NameToAssignedNumberMap* getAssignedNumberForCharacteristicNameMap()
{
    AtomicallyInitializedStaticReference(NameToAssignedNumberMap, characteristicsMap, []() {
        // https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
        NameToAssignedNumberMap* characteristics = new NameToAssignedNumberMap();
        characteristics->add("aerobic_heart_rate_lower_limit", 0x2A7E);
        characteristics->add("aerobic_heart_rate_upper_limit", 0x2A84);
        characteristics->add("aerobic_threshold", 0x2A7F);
        characteristics->add("age", 0x2A80);
        characteristics->add("alert_category_id", 0x2A43);
        characteristics->add("alert_category_id_bit_mask", 0x2A42);
        characteristics->add("alert_level", 0x2A06);
        characteristics->add("alert_notification_control_point", 0x2A44);
        characteristics->add("alert_status", 0x2A3F);
        characteristics->add("altitude", 0x2AB3);
        characteristics->add("anaerobic_heart_rate_lower_limit", 0x2A81);
        characteristics->add("anaerobic_heart_rate_upper_limit", 0x2A82);
        characteristics->add("anaerobic_threshold", 0x2A83);
        characteristics->add("apparent_wind_direction", 0x2A73);
        characteristics->add("apparent_wind_speed", 0x2A72);
        characteristics->add("appearance", 0x2A01);
        characteristics->add("barometric_pressure_trend", 0x2AA3);
        characteristics->add("battery_level", 0x2A19);
        characteristics->add("blood_pressure_feature", 0x2A49);
        characteristics->add("blood_pressure_measurement", 0x2A35);
        characteristics->add("body_composition_feature", 0x2A9B);
        characteristics->add("body_composition_measurement", 0x2A9C);
        characteristics->add("body_sensor_location", 0x2A38);
        characteristics->add("bond_management_control_point", 0x2AA4);
        characteristics->add("bond_management_feature", 0x2AA5);
        characteristics->add("boot_keyboard_input_report", 0x2A22);
        characteristics->add("boot_keyboard_output_report", 0x2A32);
        characteristics->add("boot_mouse_input_report", 0x2A33);
        characteristics->add("central_address_resolution_support", 0x2AA6);
        characteristics->add("cgm_feature", 0x2AA8);
        characteristics->add("cgm_measurement", 0x2AA7);
        characteristics->add("cgm_session_run_time", 0x2AAB);
        characteristics->add("cgm_session_start_time", 0x2AAA);
        characteristics->add("cgm_specific_ops_control_point", 0x2AAC);
        characteristics->add("cgm_status", 0x2AA9);
        characteristics->add("csc_feature", 0x2A5C);
        characteristics->add("csc_measurement", 0x2A5B);
        characteristics->add("current_time", 0x2A2B);
        characteristics->add("cycling_power_control_point", 0x2A66);
        characteristics->add("cycling_power_feature", 0x2A65);
        characteristics->add("cycling_power_measurement", 0x2A63);
        characteristics->add("cycling_power_vector", 0x2A64);
        characteristics->add("database_change_increment", 0x2A99);
        characteristics->add("date_of_birth", 0x2A85);
        characteristics->add("date_of_threshold_assessment", 0x2A86);
        characteristics->add("date_time", 0x2A08);
        characteristics->add("day_date_time", 0x2A0A);
        characteristics->add("day_of_week", 0x2A09);
        characteristics->add("descriptor_value_changed", 0x2A7D);
        characteristics->add("device_name", 0x2A00);
        characteristics->add("dew_point", 0x2A7B);
        characteristics->add("dst_offset", 0x2A0D);
        characteristics->add("elevation", 0x2A6C);
        characteristics->add("email_address", 0x2A87);
        characteristics->add("exact_time_256", 0x2A0C);
        characteristics->add("fat_burn_heart_rate_lower_limit", 0x2A88);
        characteristics->add("fat_burn_heart_rate_upper_limit", 0x2A89);
        characteristics->add("firmware_revision_string", 0x2A26);
        characteristics->add("first_name", 0x2A8A);
        characteristics->add("five_zone_heart_rate_limits", 0x2A8B);
        characteristics->add("floor_number", 0x2AB2);
        characteristics->add("gender", 0x2A8C);
        characteristics->add("glucose_feature", 0x2A51);
        characteristics->add("glucose_measurement", 0x2A18);
        characteristics->add("glucose_measurement_context", 0x2A34);
        characteristics->add("gust_factor", 0x2A74);
        characteristics->add("hardware_revision_string", 0x2A27);
        characteristics->add("heart_rate_control_point", 0x2A39);
        characteristics->add("heart_rate_max", 0x2A8D);
        characteristics->add("heart_rate_measurement", 0x2A37);
        characteristics->add("heat_index", 0x2A7A);
        characteristics->add("height", 0x2A8E);
        characteristics->add("hid_control_point", 0x2A4C);
        characteristics->add("hid_information", 0x2A4A);
        characteristics->add("hip_circumference", 0x2A8F);
        characteristics->add("humidity", 0x2A6F);
        characteristics->add("ieee_11073-20601_regulatory_certification_data_list", 0x2A2A);
        characteristics->add("indoor_positioning_configuration", 0x2AAD);
        characteristics->add("intermediate_blood_pressure", 0x2A36);
        characteristics->add("intermediate_temperature", 0x2A1E);
        characteristics->add("irradiance", 0x2A77);
        characteristics->add("language", 0x2AA2);
        characteristics->add("last_name", 0x2A90);
        characteristics->add("latitude", 0x2AAE);
        characteristics->add("ln_control_point", 0x2A6B);
        characteristics->add("ln_feature", 0x2A6A);
        characteristics->add("local_east_coordinate.xml", 0x2AB1);
        characteristics->add("local_north_coordinate", 0x2AB0);
        characteristics->add("local_time_information", 0x2A0F);
        characteristics->add("location_and_speed", 0x2A67);
        characteristics->add("location_name", 0x2AB5);
        characteristics->add("longitude", 0x2AAF);
        characteristics->add("magnetic_declination", 0x2A2C);
        characteristics->add("magnetic_flux_density_2D", 0x2AA0);
        characteristics->add("magnetic_flux_density_3D", 0x2AA1);
        characteristics->add("manufacturer_name_string", 0x2A29);
        characteristics->add("maximum_recommended_heart_rate", 0x2A91);
        characteristics->add("measurement_interval", 0x2A21);
        characteristics->add("model_number_string", 0x2A24);
        characteristics->add("navigation", 0x2A68);
        characteristics->add("new_alert", 0x2A46);
        characteristics->add("peripheral_preferred_connection_parameters", 0x2A04);
        characteristics->add("peripheral_privacy_flag", 0x2A02);
        characteristics->add("pnp_id", 0x2A50);
        characteristics->add("pollen_concentration", 0x2A75);
        characteristics->add("position_quality", 0x2A69);
        characteristics->add("pressure", 0x2A6D);
        characteristics->add("protocol_mode", 0x2A4E);
        characteristics->add("rainfall", 0x2A78);
        characteristics->add("reconnection_address", 0x2A03);
        characteristics->add("record_access_control_point", 0x2A52);
        characteristics->add("reference_time_information", 0x2A14);
        characteristics->add("report", 0x2A4D);
        characteristics->add("report_map", 0x2A4B);
        characteristics->add("resting_heart_rate", 0x2A92);
        characteristics->add("ringer_control_point", 0x2A40);
        characteristics->add("ringer_setting", 0x2A41);
        characteristics->add("rsc_feature", 0x2A54);
        characteristics->add("rsc_measurement", 0x2A53);
        characteristics->add("sc_control_point", 0x2A55);
        characteristics->add("scan_interval_window", 0x2A4F);
        characteristics->add("scan_refresh", 0x2A31);
        characteristics->add("sensor_location", 0x2A5D);
        characteristics->add("serial_number_string", 0x2A25);
        characteristics->add("gatt.service_changed", 0x2A05);
        characteristics->add("software_revision_string", 0x2A28);
        characteristics->add("sport_type_for_aerobic_and_anaerobic_thresholds", 0x2A93);
        characteristics->add("supported_new_alert_category", 0x2A47);
        characteristics->add("supported_unread_alert_category", 0x2A48);
        characteristics->add("system_id", 0x2A23);
        characteristics->add("temperature", 0x2A6E);
        characteristics->add("temperature_measurement", 0x2A1C);
        characteristics->add("temperature_type", 0x2A1D);
        characteristics->add("three_zone_heart_rate_limits", 0x2A94);
        characteristics->add("time_accuracy", 0x2A12);
        characteristics->add("time_source", 0x2A13);
        characteristics->add("time_update_control_point", 0x2A16);
        characteristics->add("time_update_state", 0x2A17);
        characteristics->add("time_with_dst", 0x2A11);
        characteristics->add("time_zone", 0x2A0E);
        characteristics->add("true_wind_direction", 0x2A71);
        characteristics->add("true_wind_speed", 0x2A70);
        characteristics->add("two_zone_heart_rate_limit", 0x2A95);
        characteristics->add("tx_power_level", 0x2A07);
        characteristics->add("uncertainty", 0x2AB4);
        characteristics->add("unread_alert_status", 0x2A45);
        characteristics->add("user_control_point", 0x2A9F);
        characteristics->add("user_index", 0x2A9A);
        characteristics->add("uv_index", 0x2A76);
        characteristics->add("vo2_max", 0x2A96);
        characteristics->add("waist_circumference", 0x2A97);
        characteristics->add("weight", 0x2A98);
        characteristics->add("weight_measurement", 0x2A9D);
        characteristics->add("weight_scale_feature", 0x2A9E);
        characteristics->add("wind_chill", 0x2A79);
        return characteristics;
    }());

    return &characteristicsMap;
}

NameToAssignedNumberMap* getAssignedNumberForDescriptorNameMap()
{
    AtomicallyInitializedStaticReference(NameToAssignedNumberMap, descriptorsMap, []() {
        // https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorsHomePage.aspx
        NameToAssignedNumberMap* descriptors = new NameToAssignedNumberMap();
        descriptors->add("characteristic_extended_properties", 0x2900);
        descriptors->add("characteristic_user_description", 0x2901);
        descriptors->add("client_characteristic_configuration", 0x2902);
        descriptors->add("server_characteristic_configuration", 0x2903);
        descriptors->add("characteristic_presentation_format", 0x2904);
        descriptors->add("characteristic_aggregate_format", 0x2905);
        descriptors->add("valid_range", 0x2906);
        descriptors->add("external_report_reference", 0x2907);
        descriptors->add("report_reference", 0x2908);
        descriptors->add("es_configuration", 0x290B);
        descriptors->add("es_measurement", 0x290C);
        descriptors->add("es_trigger_setting", 0x290D);
        return descriptors;
    }());

    return &descriptorsMap;
}

String getUUIDForGATTAttribute(GATTAttribute attribute, StringOrUnsignedLong name, ExceptionState& exceptionState)
{
    // Implementation of BluetoothUUID.getService, BluetoothUUID.getCharacteristic
    // and BluetoothUUID.getDescriptor algorithms:
    // https://webbluetoothcg.github.io/web-bluetooth/#dom-bluetoothuuid-getservice
    // https://webbluetoothcg.github.io/web-bluetooth/#dom-bluetoothuuid-getcharacteristic
    // https://webbluetoothcg.github.io/web-bluetooth/#dom-bluetoothuuid-getdescriptor

    // If name is an unsigned long, return BluetoothUUID.cannonicalUUI(name) and
    // abort this steps.
    if (name.isUnsignedLong())
        return BluetoothUUID::canonicalUUID(name.getAsUnsignedLong());

    String nameStr = name.getAsString();

    // If name is a valid UUID, return name and abort these steps.
    if (isValidUUID(nameStr))
        return nameStr;

    // If name is in the corresponding attribute map return
    // BluetoothUUID.cannonicalUUID(alias).
    NameToAssignedNumberMap* map = nullptr;
    const char* attributeType = nullptr;
    switch (attribute) {
    case GATTAttribute::Service:
        map = getAssignedNumberToServiceNameMap();
        attributeType = "Service";
        break;
    case GATTAttribute::Characteristic:
        map = getAssignedNumberForCharacteristicNameMap();
        attributeType = "Characteristic";
        break;
    case GATTAttribute::Descriptor:
        map = getAssignedNumberForDescriptorNameMap();
        attributeType = "Descriptor";
        break;
    }

    if (map->contains(nameStr))
        return BluetoothUUID::canonicalUUID(map->get(nameStr));

    StringBuilder errorMessage;
    errorMessage.append("Invalid ");
    errorMessage.append(attributeType);
    errorMessage.append(" name: '");
    errorMessage.append(nameStr);
    errorMessage.append("'.");
    // Otherwise, throw a SyntaxError.
    exceptionState.throwDOMException(SyntaxError, errorMessage.toString());
    return String();
}

} // namespace

// static
String BluetoothUUID::getService(StringOrUnsignedLong name, ExceptionState& exceptionState)
{
    return getUUIDForGATTAttribute(GATTAttribute::Service, name, exceptionState);
}

// static
String BluetoothUUID::getCharacteristic(StringOrUnsignedLong name, ExceptionState& exceptionState)
{
    return getUUIDForGATTAttribute(GATTAttribute::Characteristic, name, exceptionState);
}

// static
String BluetoothUUID::getDescriptor(StringOrUnsignedLong name, ExceptionState& exceptionState)
{
    return getUUIDForGATTAttribute(GATTAttribute::Descriptor, name, exceptionState);
}

// static
String BluetoothUUID::canonicalUUID(unsigned alias)
{
    StringBuilder builder;
    builder.reserveCapacity(36 /* 36 chars or 128 bits, length of a UUID */);
    appendUnsignedAsHexFixedSize(
        alias,
        builder, 8 /* 8 chars or 32 bits, prefix length */,
        Lowercase);

    builder.append("-0000-1000-8000-00805f9b34fb");
    return builder.toString();
}

} // namespace blink
