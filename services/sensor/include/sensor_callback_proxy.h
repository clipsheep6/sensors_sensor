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

#ifndef SENSOR_CALLBACK_PROXY_H
#define SENSOR_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "nocopyable.h"

#include "i_sensor_callback.h"
#include "errors.h"

namespace OHOS {
namespace Sensors {
class SensorCallbackProxy : public IRemoteProxy<ISensorCallback> {
public:
    explicit SensorCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ISensorCallback>(impl) {}
    ~SensorCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(SensorCallbackProxy);
    void OnSensorChanged(const AppSensorInfo &appSensorInfo) override;

private:
    static inline BrokerDelegator<SensorCallbackProxy> delegator_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // SENSOR_CALLBACK_PROXY_H
