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

#include "sensor_service_impl.h"
#include <cmath>
#include <cstring>
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_SERVICE, "SensorServiceImpl" };
}

ZReportDataCb SensorServiceImpl::reportDataCb_ = nullptr;
sptr<ReportDataCallback> SensorServiceImpl::reportDataCallback_ = nullptr;
std::mutex SensorServiceImpl::dataMutex_;
std::condition_variable SensorServiceImpl::dataCondition_;

ErrCode SensorServiceImpl::InitSensorServiceImpl()
{
    HiLog::Info(LABEL, "%{public}s begin", "InitSensorServiceImpl");
    sensorInterface_ = NewSensorInterfaceInstance();
    if (sensorInterface_ == nullptr) {
        HiLog::Error(LABEL, " %{public}s,", "test sensorHdi get Module instance failed\n\r");
        return ERR_INVALID_VALUE;
    }
    HiLog::Info(LABEL, "%{public}s end", "InitSensorServiceImpl");
    return ERR_OK;
}

std::vector<Sensor> SensorServiceImpl::GetSensorList() const
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    struct SensorInformation *sensorInfo = nullptr;
    int32_t count = 0;
    int32_t ret = sensorInterface_->GetAllSensors(&sensorInfo, &count);
    if (ret != 0) {
        HiLog::Error(LABEL, "Get sensor list failed!");
        return std::vector<Sensor>();
    }
    HiLog::Info(LABEL, "GetAllSensors result: %{public}d, count: %{public}d", ret, count);
    std::vector<Sensor> list;
    for (int i = 0; i < count; i++) {
        const std::string sensorName(sensorInfo->sensorName);
        const std::string vendorName(sensorInfo->vendorName);
        const int32_t sensorId = sensorInfo->sensorId;
        const float power = sensorInfo->power;
        const float maxRange = sensorInfo->maxRange;
        HiLog::Info(LABEL, " %{public}d, %{public}s, %{public}s, %{public}f", sensorId, sensorName.c_str(), 
                    vendorName.c_str(), power);
        Sensor sensor;
        sensor.SetSensorId(sensorId);
        sensor.SetMaxRange(maxRange);
        sensor.SetName(sensorName.c_str());
        sensor.SetVendor(vendorName.c_str());
        list.push_back(sensor);
        sensorInfo++;
    }
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return list;
}

ErrCode SensorServiceImpl::EnableSensor(uint32_t sensorId) const
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = sensorInterface_->Enable(sensorId);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s is failed", __func__);
        return -1;
    }
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return ERR_OK;
}

ErrCode SensorServiceImpl::DisableSensor(uint32_t sensorId) const
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = sensorInterface_->Disable(sensorId);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s is failed", __func__);
        return -1;
    }
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return ERR_OK;
}

ErrCode SensorServiceImpl::RunCommand(uint32_t sensorId, int32_t cmd, int32_t params) const
{
    return ERR_OK;
}

ErrCode SensorServiceImpl::SetSensorConfig(uint32_t sensorId, int64_t samplingPeriodNs, int64_t maxReportDelayNs) const
{
    HiLog::Debug(LABEL, "%{public}s begin", __func__);
    int32_t ret = sensorInterface_->SetBatch(sensorId, samplingPeriodNs, maxReportDelayNs);
    if (SENSOR_NOT_SUPPORT == ret) {
        HiLog::Debug(LABEL, "%{public}s : The operation is not supported!", __func__);
        return SENSOR_NOT_SUPPORT;
    }
    if (SENSOR_INVALID_PARAM == ret) {
        HiLog::Debug(LABEL, "%{public}s : The sensor parameter is invalid", __func__);
        return SENSOR_INVALID_PARAM;
    }
    HiLog::Debug(LABEL, "%{public}s end", __func__);
    return ret;
}

int32_t SensorServiceImpl::SensorDataCallback(const struct SensorEvents *event)
{
    HiLog::Debug(LABEL, "%{public}s begin", __func__);
    const int32_t SENSOR_AXISZ = 2;
    if ((event == nullptr) || (event->dataLen == 0)) {
        HiLog::Error(LABEL, "%{public}s event is NULL", __func__);
        return ERR_INVALID_VALUE;
    }

    float *data = (float*)event->data;
    if (event->sensorId == 0) {
        printf("sensor id [%d] data [%f]\n\r", event->sensorId, *(data));
        HiLog::Info(LABEL, "sensor id: %{public}d, data: %{public}f",  event->sensorId, *(data));
        if (fabs(*data) > 1e-5) {
        }
    } else if (event->sensorId == 1) {
        printf("sensor id [%d] x-[%f] y-[%f] z-[%f]\n\r",
            event->sensorId, (*data), *(data + 1), *(data + SENSOR_AXISZ));
        HiLog::Info(LABEL, "sensor id: %{public}d x-%{public}f y-%{public}f z-%{public}f\n\r",
            event->sensorId, (*data), *(data + 1), *(data + SENSOR_AXISZ)); 
    }

    if (reportDataCb_ == nullptr) {
        HiLog::Error(LABEL, "%{public}s reportDataCb_ cannot be null", __func__);
        return ERR_INVALID_VALUE;
    }
    (void)(reportDataCallback_->*reportDataCb_)(reinterpret_cast<const struct SensorEvent*>(event), 
                                                reportDataCallback_);
    dataCondition_.notify_one();
    return ERR_OK;
}

ErrCode SensorServiceImpl::Register(RecordDataCallback cb) const
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (sensorInterface_ == nullptr) {
        HiLog::Error(LABEL, " %{public}s,", "test sensorHdi get Module instance failed\n\r");
        return ERR_INVALID_VALUE;
    }
    int32_t ret = sensorInterface_->Register(cb);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s failed", __func__);
        return ERR_INVALID_VALUE;
    }
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return ERR_OK;
}

ErrCode SensorServiceImpl::RegisteDataReport(ZReportDataCb cb, sptr<ReportDataCallback> reportDataCallback)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    if (reportDataCallback == nullptr) {
        HiLog::Error(LABEL, "%{public}s failed, reportDataCallback cannot be null", __func__);
        return ERR_NO_INIT;
    }

    Register(SensorDataCallback);

    reportDataCb_ = cb;
    reportDataCallback_ = reportDataCallback;
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return ERR_OK;
}

ErrCode SensorServiceImpl::Unregister(void) const
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    int32_t ret = sensorInterface_->Unregister();
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s failed", __func__);
        return ERR_INVALID_VALUE;
    }
    HiLog::Info(LABEL, "%{public}s end", __func__);
    return ERR_OK;
}
}  // namespace Sensors
}  // namespace OHOS
