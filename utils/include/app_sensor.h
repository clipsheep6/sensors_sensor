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

#ifndef APP_SENSOR_H
#define APP_SENSOR_H

#include <vector>

#include "parcel.h"

namespace OHOS {
namespace Sensors {
struct AppSensor : public Parcelable {
    int32_t pid { 0 };
    int32_t uid { 0 };
    uint32_t tokenId { 0 };
    uint32_t sensorId { 0 };
    int64_t samplingPeriodNs { 0 };
    int64_t maxReportDelayNs { 0 };

    AppSensor() = default;
    AppSensor(int32_t pid, int32_t uid, uint32_t tokenId, uint32_t sensorId,
              int64_t samplingPeriodNs, int64_t maxReportDelayNs)
        : pid(pid), uid(uid), tokenId(tokenId), sensorId(sensorId),
          samplingPeriodNs(samplingPeriodNs), maxReportDelayNs(maxReportDelayNs) {}
    
    bool Marshalling(Parcel &parcel) const;
    std::unique_ptr<AppSensor> Unmarshalling(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // APP_SENSOR_H
