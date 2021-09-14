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

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include "report_data_callback.h"
#include "errors.h"
#include "refbase.h"
#include "singleton.h"
#include "sensor.h"
#include "sensor_if.h"
#include "sensor_agent_type.h"
#include "nocopyable.h"

namespace OHOS {
namespace Sensors {
class SensorServiceImpl : public Singleton<SensorServiceImpl> {
public:
    SensorServiceImpl() = default;
    virtual ~SensorServiceImpl() = default;
    std::vector<Sensor> GetSensorList() const;

    ErrCode EnableSensor(uint32_t sensorId) const;

    ErrCode DisableSensor(uint32_t sensorId) const;

    ErrCode RunCommand(uint32_t sensorId, int32_t cmd, int32_t params) const;

    ErrCode SetSensorConfig(uint32_t sensorId, int64_t samplingPeriodNs, int64_t maxReportDelayNs) const;

    ErrCode InitSensorServiceImpl();

    ErrCode RegisteDataReport(ZReportDataCb cb, sptr<ReportDataCallback> reportDataCallback);

    ErrCode Unregister(void) const;

    static int32_t SensorDataCallback(const struct SensorEvents *event);
    static std::mutex dataMutex_;
    static std::condition_variable dataCondition_;

private:
    DISALLOW_COPY_AND_MOVE(SensorServiceImpl);
    const struct SensorInterface *sensorInterface_ = nullptr;
    ErrCode Register(RecordDataCallback cb) const;
    static ZReportDataCb reportDataCb_;
    static sptr<ReportDataCallback> reportDataCallback_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // SENSOR_INTERFACE_H
