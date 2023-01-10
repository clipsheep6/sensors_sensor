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

#ifndef SENSOR_CALLBACK_STUB_H
#define SENSOR_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "nocopyable.h"

#include "i_sensor_callback.h"

namespace OHOS {
namespace Sensors {
class SensorCallbackStub : public IRemoteStub<ISensorCallback> {
public:
    DISALLOW_COPY_AND_MOVE(SensorCallbackStub);
    SensorCallbackStub() = default;
    virtual ~SensorCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnSensorChanged(const AppSensorInfo &appSensorInfo) override {}
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // SENSOR_CALLBACK_STUB_H
