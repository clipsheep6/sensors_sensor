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
#include "compatible_connection.h"

#include "securec.h"
#include "sensor_errors.h"

#undef LOG_TAG
#define LOG_TAG "CompatibleConnection"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

ReportDataCb CompatibleConnection::reportDataCb_ = nullptr;
sptr<ReportDataCallback> CompatibleConnection::reportDataCallback_ = nullptr;
int32_t CompatibleConnection::ConnectHdi()
{
    SEN_HILOGI("Connect hdi success");
    return ERR_OK;
}

int32_t CompatibleConnection::GetSensorList(std::vector<Sensor> &sensorList)
{
    std::vector<SensorInfo> sensorInfos;
    int32_t ret = hdiServiceImpl_.GetSensorList(sensorInfos);
    if (ret != 0) {
        SEN_HILOGE("Get sensor list failed");
        return ret;
    }
    size_t count = sensorInfos.size();
    if (count > MAX_SENSOR_COUNT) {
        SEN_HILOGD("SensorInfos size:%{public}zu", count);
        count = MAX_SENSOR_COUNT;
    }
    for (size_t i = 0; i < count; i++) {
        const std::string sensorName(sensorInfos[i].sensorName);
        const std::string vendorName(sensorInfos[i].vendorName);
        const std::string firmwareVersion(sensorInfos[i].firmwareVersion);
        const std::string hardwareVersion(sensorInfos[i].hardwareVersion);
        const int32_t sensorId = sensorInfos[i].sensorIndex;
        const int32_t sensorTypeId = sensorInfos[i].sensorTypeId;
        const int32_t deviceId = sensorInfos[i].deviceId;
        const int32_t location = sensorInfos[i].location;
        const float maxRange = sensorInfos[i].maxRange;
        Sensor sensor;
        sensor.SetDeviceId(deviceId);
        sensor.SetSensorId(sensorId);
        sensor.SetSensorTypeId(sensorTypeId);
        sensor.SetLocation(location);
        sensor.SetFirmwareVersion(firmwareVersion);
        sensor.SetHardwareVersion(hardwareVersion);
        sensor.SetMaxRange(maxRange);
        sensor.SetSensorName(sensorName);
        sensor.SetVendorName(vendorName);
        sensor.SetResolution(sensorInfos[i].precision);
        sensor.SetPower(sensorInfos[i].power);
        sensor.SetMinSamplePeriodNs(sensorInfos[i].minSamplePeriod);
        sensor.SetMaxSamplePeriodNs(sensorInfos[i].maxSamplePeriod);
        sensorList.push_back(sensor);
    }
    return ERR_OK;
}

int32_t CompatibleConnection::EnableSensor(const SensorDescription &sensorDesc)
{
    int32_t ret = hdiServiceImpl_.EnableSensor(sensorDesc);
    if (ret != 0) {
        SEN_HILOGE("Enable sensor failed, deviceId:%{public}d, sensortypeId:%{public}d, sensorId:%{public}d",
            sensorDesc.deviceId, sensorDesc.sensorType, sensorDesc.sensorId);
        return ret;
    }
    return ERR_OK;
};

int32_t CompatibleConnection::DisableSensor(const SensorDescription &sensorDesc)
{
    int32_t ret = hdiServiceImpl_.DisableSensor(sensorDesc);
    if (ret != 0) {
        SEN_HILOGE("Disable sensor failed, deviceId:%{public}d, sensortypeId:%{public}d, sensorId:%{public}d",
            sensorDesc.deviceId, sensorDesc.sensorType, sensorDesc.sensorId);
        return ret;
    }
    return ERR_OK;
}

int32_t CompatibleConnection::SetBatch(const SensorDescription &sensorDesc, int64_t samplingInterval,
    int64_t reportInterval)
{
    int32_t ret = hdiServiceImpl_.SetBatch(sensorDesc, samplingInterval, reportInterval);
    if (ret != 0) {
        SEN_HILOGE("Set batch failed, deviceId:%{public}d, sensortypeId:%{public}d, sensorId:%{public}d",
            sensorDesc.deviceId, sensorDesc.sensorType, sensorDesc.sensorId);
        return ret;
    }
    return ERR_OK;
}

int32_t CompatibleConnection::SetMode(const SensorDescription &sensorDesc, int32_t mode)
{
    int32_t ret = hdiServiceImpl_.SetMode(sensorDesc, mode);
    if (ret != 0) {
        SEN_HILOGI("Set mode failed, deviceId:%{public}d, sensortypeId:%{public}d, sensorId:%{public}d",
            sensorDesc.deviceId, sensorDesc.sensorType, sensorDesc.sensorId);
        return ret;
    }
    return ERR_OK;
}

void CompatibleConnection::ReportSensorDataCallback(SensorEvent *event)
{
    CHKPV(event);
    if ((event->dataLen) == 0) {
        SEN_HILOGE("Event is NULL");
        return;
    }

    SensorData sensorData = {
        .sensorTypeId = event->sensorTypeId,
        .version = event->version,
        .timestamp = event->timestamp,
        .option = event->option,
        .mode = event->mode,
        .dataLen = event->dataLen,
        .deviceId = event->deviceId,
        .sensorId = event->sensorId,
        .location = event->location
    };
    CHKPV(sensorData.data);
    errno_t ret = memcpy_s(sensorData.data, sizeof(sensorData.data), event->data, event->dataLen);
    if (ret != EOK) {
        SEN_HILOGE("Copy data failed");
        return;
    }
    CHKPV(reportDataCallback_);
    CHKPV(reportDataCb_);
    std::unique_lock<std::mutex> lk(ISensorHdiConnection::dataMutex_);
    (void)(reportDataCallback_->*reportDataCb_)(&sensorData, reportDataCallback_);
    ISensorHdiConnection::dataReady_.store(true);
    ISensorHdiConnection::dataCondition_.notify_one();
}

int32_t CompatibleConnection::RegisterDataReport(ReportDataCb cb, sptr<ReportDataCallback> reportDataCallback)
{
    CHKPR(reportDataCallback, ERR_INVALID_VALUE);
    int32_t ret = hdiServiceImpl_.Register(ReportSensorDataCallback);
    if (ret != 0) {
        SEN_HILOGE("Register is failed");
        return ret;
    }
    reportDataCb_ = cb;
    reportDataCallback_ = reportDataCallback;
    return ERR_OK;
}

int32_t CompatibleConnection::DestroyHdiConnection()
{
    int32_t ret = hdiServiceImpl_.Unregister();
    if (ret != 0) {
        SEN_HILOGE("Unregister is failed");
        return ret;
    }
    return ERR_OK;
}

int32_t CompatibleConnection::GetSensorListByDevice(int32_t deviceId, std::vector<Sensor> &singleDevSensors)
{
    std::vector<SensorInfo> sensorInfos;
    int32_t ret = hdiServiceImpl_.GetSensorListByDevice(deviceId, sensorInfos);
    if (ret != 0) {
        SEN_HILOGE("Get sensor list by device failed");
        return ret;
    }
    size_t count = sensorInfos.size();
    if (count > MAX_SENSOR_COUNT) {
        SEN_HILOGD("SensorInfos size:%{public}zu", count);
        count = MAX_SENSOR_COUNT;
    }
    for (size_t i = 0; i < count; i++) {
        const std::string sensorName(sensorInfos[i].sensorName);
        const std::string vendorName(sensorInfos[i].vendorName);
        const std::string firmwareVersion(sensorInfos[i].firmwareVersion);
        const std::string hardwareVersion(sensorInfos[i].hardwareVersion);
        const int32_t sensorId = sensorInfos[i].sensorIndex;
        const int32_t sensorTypeId = sensorInfos[i].sensorTypeId;
        const int32_t deviceId = sensorInfos[i].deviceId;
        const int32_t location = sensorInfos[i].location;
        const float maxRange = sensorInfos[i].maxRange;
        Sensor sensor;
        sensor.SetDeviceId(deviceId);
        sensor.SetSensorId(sensorId);
        sensor.SetSensorTypeId(sensorTypeId);
        sensor.SetLocation(location);
        sensor.SetFirmwareVersion(firmwareVersion);
        sensor.SetHardwareVersion(hardwareVersion);
        sensor.SetMaxRange(maxRange);
        sensor.SetSensorName(sensorName);
        sensor.SetVendorName(vendorName);
        sensor.SetResolution(sensorInfos[i].precision);
        sensor.SetPower(sensorInfos[i].power);
        sensor.SetMinSamplePeriodNs(sensorInfos[i].minSamplePeriod);
        sensor.SetMaxSamplePeriodNs(sensorInfos[i].maxSamplePeriod);
        singleDevSensors.push_back(sensor);
    }
    return ERR_OK;
}

int32_t CompatibleConnection::RegSensorPlugCallback(DevicePlugCallback cb)
{
    return ERR_OK;
}

DevicePlugCallback CompatibleConnection::GetSensorPlugCb()
{
    return NULL;
}
} // namespace Sensors
} // namespace OHOS