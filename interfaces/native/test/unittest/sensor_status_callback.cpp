/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "sensor_status_callback.h"

#include "hisysevent.h"

#include "sensor_power_test.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
void SensorStatusCallback::OnSensorChanged(const AppSensorInfo &appSensorInfo)
{
    g_callback_pid = appSensorInfo.pid;
    g_callback_sensorId = appSensorInfo.sensorId;
    g_callback_isActive = appSensorInfo.isActive;
    g_callback_samplingPeriodNs = appSensorInfo.samplingPeriodNs;
    g_callback_maxReportDelayNs = appSensorInfo.maxReportDelayNs;
}
}  // namespace Sensors
}  // namespace OHOS
