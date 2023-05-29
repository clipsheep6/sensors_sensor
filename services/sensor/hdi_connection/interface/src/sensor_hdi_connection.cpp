/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sensor_hdi_connection.h"

#include "compatible_connection.h"
#include "hdi_connection.h"
#include "hitrace_meter.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorHdiConnection" };
}

int32_t SensorHdiConnection::ConnectHdi()
{
    iSensorHdiConnection_ = std::make_unique<HdiConnection>();
    int32_t ret = ConnectHdiService();
    if (ret != ERR_OK) {
        SEN_HILOGE("connect hdi service failed, try to connect compatible connection");
        iSensorHdiConnection_ = std::make_unique<CompatibleConnection>();
        ret = ConnectHdiService();
    }
    if (ret != ERR_OK) {
        SEN_HILOGE("connect hdi failed");
    }
    ret = ConnectCompatibleHdi();
    if (ret != ERR_OK) {
        SEN_HILOGE("connect compatible hdi failed");
    }
    return ERR_OK;
}

int32_t SensorHdiConnection::ConnectHdiService()
{
    int32_t ret = iSensorHdiConnection_->ConnectHdi();
    if (ret != 0) {
        SEN_HILOGE("connect hdi service failed");
        return CONNECT_SENSOR_HDF_ERR;
    }
    ret = iSensorHdiConnection_->GetSensorList(sensorList_);
    if (ret != 0) {
        SEN_HILOGE("get sensor list failed");
        return GET_SENSOR_LIST_ERR;
    }
    return ERR_OK;
}

int32_t SensorHdiConnection::ConnectCompatibleHdi()
{
    iSensorCompatibleHdiConnection_ = std::make_unique<CompatibleConnection>();
    int32_t ret = iSensorCompatibleHdiConnection_->ConnectHdi();
    if (ret != ERR_OK) {
        SEN_HILOGE("connect hdi compatible service failed");
        return CONNECT_SENSOR_HDF_ERR;
    }
    return ERR_OK;
}

int32_t SensorHdiConnection::GetSensorList(std::vector<Sensor>& sensorList)
{
    sensorList.assign(sensorList_.begin(), sensorList_.end());
    Sensor sensor1;
    sensor1.SetSensorId(14);
    sensor1.SetSensorTypeId(14);
    sensor1.SetFirmwareVersion("1.0.1");
    sensor1.SetHardwareVersion("1.0.1");
    sensor1.SetMaxRange(9999.0);
    sensor1.SetSensorName("sensor_color");
    sensor1.SetVendorName("default_color");
    sensor1.SetResolution(0.000001);
    sensor1.SetPower(20.0);
    sensor1.SetMinSamplePeriodNs(100000000);
    sensor1.SetMaxSamplePeriodNs(1000000000);
    sensorList.push_back(sensor1);
    Sensor sensor2;
    sensor2.SetSensorId(15);
    sensor2.SetSensorTypeId(15);
    sensor2.SetFirmwareVersion("1.0.2");
    sensor2.SetHardwareVersion("1.0.2");
    sensor2.SetMaxRange(9999.0);
    sensor2.SetSensorName("sensor_sar");
    sensor2.SetVendorName("default_sar");
    sensor2.SetResolution(0.000002);
    sensor2.SetPower(21.0);
    sensor2.SetMinSamplePeriodNs(100000000);
    sensor2.SetMaxSamplePeriodNs(1000000000);
    sensorList.push_back(sensor2);
    return ERR_OK;
}

int32_t SensorHdiConnection::EnableSensor(int32_t sensorId)
{
    StartTrace(HITRACE_TAG_SENSORS, "EnableSensor");
    int32_t ret { ENABLE_SENSOR_ERR };
    if (sensorId == SENSOR_TYPE_ID_COLOR || sensorId == SENSOR_TYPE_ID_SAR) {
        ret = iSensorCompatibleHdiConnection_->EnableSensor(sensorId);
        FinishTrace(HITRACE_TAG_SENSORS);
        if (ret != ERR_OK) {
            SEN_HILOGE("enable sensor failed in compatible, sensorId:%{public}d", sensorId);
            return ENABLE_SENSOR_ERR;
        }
        return ret;
    }
    ret = iSensorHdiConnection_->EnableSensor(sensorId);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != 0) {
        SEN_HILOGI("enable sensor failed, sensorId:%{public}d", sensorId);
        return ENABLE_SENSOR_ERR;
    }
    return ret;
};

int32_t SensorHdiConnection::DisableSensor(int32_t sensorId)
{
    StartTrace(HITRACE_TAG_SENSORS, "DisableSensor");
    int32_t ret { ENABLE_SENSOR_ERR };
    if (sensorId == SENSOR_TYPE_ID_COLOR || sensorId == SENSOR_TYPE_ID_SAR) {
        ret = iSensorCompatibleHdiConnection_->DisableSensor(sensorId);
        FinishTrace(HITRACE_TAG_SENSORS);
        if (ret != ERR_OK) {
            SEN_HILOGE("disable sensor failed in compatible, sensorId:%{public}d", sensorId);
            return DISABLE_SENSOR_ERR;
        }
        return ret;
    }
    ret = iSensorHdiConnection_->DisableSensor(sensorId);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != 0) {
        SEN_HILOGI("disable sensor failed, sensorId:%{public}d", sensorId);
        return DISABLE_SENSOR_ERR;
    }
    return ret;
}

int32_t SensorHdiConnection::SetBatch(int32_t sensorId, int64_t samplingInterval, int64_t reportInterval)
{
    StartTrace(HITRACE_TAG_SENSORS, "SetBatch");
    int32_t ret { ENABLE_SENSOR_ERR };
    if (sensorId == SENSOR_TYPE_ID_COLOR || sensorId == SENSOR_TYPE_ID_SAR) {
        ret = iSensorCompatibleHdiConnection_->SetBatch(sensorId, samplingInterval, reportInterval);
        FinishTrace(HITRACE_TAG_SENSORS);
        if (ret != ERR_OK) {
            SEN_HILOGI("set batch failed in compatible, sensorId:%{public}d", sensorId);
            return SET_SENSOR_CONFIG_ERR;
        }
        return ret;
    }
    ret = iSensorHdiConnection_->SetBatch(sensorId, samplingInterval, reportInterval);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != 0) {
        SEN_HILOGI("set batch failed, sensorId:%{public}d", sensorId);
        return SET_SENSOR_CONFIG_ERR;
    }
    return ret;
}

int32_t SensorHdiConnection::SetMode(int32_t sensorId, int32_t mode)
{
    StartTrace(HITRACE_TAG_SENSORS, "SetMode");
    int32_t ret = iSensorHdiConnection_->SetMode(sensorId, mode);
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != 0) {
        SEN_HILOGI("set mode failed, sensorId:%{public}d", sensorId);
        return SET_SENSOR_MODE_ERR;
    }
    return ret;
}

int32_t SensorHdiConnection::RegisteDataReport(ReportDataCb cb, sptr<ReportDataCallback> reportDataCallback)
{
    StartTrace(HITRACE_TAG_SENSORS, "RegisteDataReport");
    int32_t ret = iSensorHdiConnection_->RegisteDataReport(cb, reportDataCallback);
    int32_t compatibleRet = iSensorCompatibleHdiConnection_->RegisteDataReport(cb, reportDataCallback);
    if (compatibleRet != ERR_OK) {
        SEN_HILOGE("registe dataReport failed in compatible");
    }
    FinishTrace(HITRACE_TAG_SENSORS);
    if (ret != 0) {
        SEN_HILOGI("registe dataReport failed");
        return REGIST_CALLBACK_ERR;
    }
    return ret;
}

int32_t SensorHdiConnection::DestroyHdiConnection()
{
    int32_t ret = iSensorHdiConnection_->DestroyHdiConnection();
    if (ret != 0) {
        SEN_HILOGI("destroy hdi connection failed");
        return DEVICE_ERR;
    }
    int32_t compatibleRet = iSensorCompatibleHdiConnection_->DestroyHdiConnection();
    if (compatibleRet != ERR_OK) {
        SEN_HILOGE("destroy hdi connection failed in compatible");
    }
    return ret;
}
}  // namespace Sensors
}  // namespace OHOS
