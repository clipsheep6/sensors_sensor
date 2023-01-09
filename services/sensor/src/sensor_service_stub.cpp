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

#include "sensor_service_stub.h"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "message_parcel.h"
#include "accesstoken_kit.h"
#include "permission_util.h"
#include "sensor_client_proxy.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
using namespace Security::AccessToken;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorServiceStub" };
}  // namespace

SensorServiceStub::SensorServiceStub()
{
    CALL_LOG_ENTER;
    baseFuncs_[ENABLE_SENSOR] = &SensorServiceStub::SensorEnableInner;
    baseFuncs_[DISABLE_SENSOR] = &SensorServiceStub::SensorDisableInner;
    baseFuncs_[GET_SENSOR_LIST] = &SensorServiceStub::GetAllSensorsInner;
    baseFuncs_[TRANSFER_DATA_CHANNEL] = &SensorServiceStub::CreateDataChannelInner;
    baseFuncs_[DESTROY_SENSOR_CHANNEL] = &SensorServiceStub::DestroyDataChannelInner;
    baseFuncs_[SUSPEND_SENSORS] = &SensorServiceStub::SuspendSensorsInner;
    baseFuncs_[RESUME_SENSORS] = &SensorServiceStub::ResumeSensorsInner;
    baseFuncs_[GET_APP_SENSOR_LIST] = &SensorServiceStub::GetAppSensorListInner;
    baseFuncs_[REGISTER_CALLBACK] = &SensorServiceStub::RegisterCallbackInner;
}

SensorServiceStub::~SensorServiceStub()
{
    CALL_LOG_ENTER;
    baseFuncs_.clear();
}

int32_t SensorServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                           MessageOption &option)
{
    SEN_HILOGD("begin, cmd : %{public}u", code);
    std::u16string descriptor = SensorServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        SEN_HILOGE("client and service descriptors are inconsistent");
        return OBJECT_NULL;
    }
    auto itFunc = baseFuncs_.find(code);
    if (itFunc != baseFuncs_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    SEN_HILOGD("no member func supporting, applying default process");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode SensorServiceStub::SensorEnableInner(MessageParcel &data, MessageParcel &reply)
{
    (void)reply;
    uint32_t sensorId;
    if (!data.ReadUint32(sensorId)) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckSensorPermission(GetCallingTokenID(), sensorId);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::SENSOR, "SENSOR_VERIFY_ACCESS_TOKEN_FAIL",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "SensorEnableInner", "ERROR_CODE", ret);
        SEN_HILOGE("sensorId:%{public}u grant failed,result:%{public}d", sensorId, ret);
        return PERMISSION_DENIED;
    }
    int64_t samplingPeriodNs;
    int64_t maxReportDelayNs;
    if ((!data.ReadInt64(samplingPeriodNs)) || (!data.ReadInt64(maxReportDelayNs))) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    return EnableSensor(sensorId, samplingPeriodNs, maxReportDelayNs);
}

ErrCode SensorServiceStub::SensorDisableInner(MessageParcel &data, MessageParcel &reply)
{
    (void)reply;
    uint32_t sensorId;
    if (!data.ReadUint32(sensorId)) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    PermissionUtil &permissionUtil = PermissionUtil::GetInstance();
    int32_t ret = permissionUtil.CheckSensorPermission(GetCallingTokenID(), sensorId);
    if (ret != PERMISSION_GRANTED) {
        HiSysEventWrite(HiSysEvent::Domain::SENSOR, "SENSOR_VERIFY_ACCESS_TOKEN_FAIL",
            HiSysEvent::EventType::SECURITY, "PKG_NAME", "SensorDisableInner", "ERROR_CODE", ret);
        SEN_HILOGE("sensorId:%{public}u grant failed,result:%{public}d", sensorId, ret);
        return PERMISSION_DENIED;
    }
    return DisableSensor(sensorId);
}

ErrCode SensorServiceStub::GetAllSensorsInner(MessageParcel &data, MessageParcel &reply)
{
    (void)data;
    std::vector<Sensor> sensors(GetSensorList());
    int32_t sensorCount = int32_t { sensors.size() };
    reply.WriteInt32(sensorCount);
    for (int32_t i = 0; i < sensorCount; i++) {
        bool flag = sensors[i].Marshalling(reply);
        if (!flag) {
            SEN_HILOGE("sensor %{public}d failed", i);
            return GET_SENSOR_LIST_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode SensorServiceStub::CreateDataChannelInner(MessageParcel &data, MessageParcel &reply)
{
    (void)reply;
    sptr<SensorBasicDataChannel> sensorChannel = new (std::nothrow) SensorBasicDataChannel();
    CHKPR(sensorChannel, OBJECT_NULL);
    auto ret = sensorChannel->CreateSensorBasicChannel(data);
    if (ret != ERR_OK) {
        SEN_HILOGE("CreateSensorBasicChannel ret:%{public}d", ret);
        return OBJECT_NULL;
    }
    sptr<IRemoteObject> sensorClient = data.ReadRemoteObject();
    CHKPR(sensorClient, OBJECT_NULL);
    return TransferDataChannel(sensorChannel, sensorClient);
}

ErrCode SensorServiceStub::DestroyDataChannelInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> sensorClient = data.ReadRemoteObject();
    CHKPR(sensorClient, OBJECT_NULL);
    return DestroySensorChannel(sensorClient);
}

ErrCode SensorServiceStub::SuspendSensorsInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(GetCallingTokenID());
    if (tokenType != ATokenTypeEnum::TOKEN_NATIVE) {
        SEN_HILOGE("tokenType is not TOKEN_NATIVE, tokenType:%{public}d", tokenType);
        // return PERMISSION_DENIED;
    }
    (void)reply;
    int32_t pid;
    if (!data.ReadInt32(pid)) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    return SuspendSensors(pid);
}

ErrCode SensorServiceStub::ResumeSensorsInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(GetCallingTokenID());
    if (tokenType != ATokenTypeEnum::TOKEN_NATIVE) {
        SEN_HILOGE("tokenType is not TOKEN_NATIVE, tokenType:%{public}d", tokenType);
        // return PERMISSION_DENIED;
    }
    (void)reply;
    int32_t pid;
    if (!data.ReadInt32(pid)) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    return ResumeSensors(pid);
}

ErrCode SensorServiceStub::GetAppSensorListInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(GetCallingTokenID());
    if (tokenType != ATokenTypeEnum::TOKEN_NATIVE) {
        SEN_HILOGE("tokenType is not TOKEN_NATIVE, tokenType:%{public}d", tokenType);
        // return PERMISSION_DENIED;
    }
    (void)data;
    std::vector<AppSensor> appSensors = GetAppSensorList();
    int32_t appSensorCount = int32_t { appSensors.size() };
    reply.WriteInt32(appSensorCount);
    for (int32_t i = 0; i < appSensorCount; i++) {
        bool flag = appSensors[i].Marshalling(reply);
        if (!flag) {
            SEN_HILOGE("appSensor %{public}d failed", i);
            return ERROR;
        }
    }
    return NO_ERROR;
}

ErrCode SensorServiceStub::RegisterCallbackInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(GetCallingTokenID());
    if (tokenType != ATokenTypeEnum::TOKEN_NATIVE) {
        SEN_HILOGE("tokenType is not TOKEN_NATIVE, tokenType:%{public}d", tokenType);
        // return PERMISSION_DENIED;
    }
    (void)reply;
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        SEN_HILOGE("data ReadRemoteObject() is nullptr");
        return ERROR;
    }
    sptr<ISensorCallback> callback = iface_cast<ISensorCallback>(obj);
    if (callback == nullptr) {
        SEN_HILOGE("obj iface_cast ISensorCallback failed");
        return ERROR;
    }
    return RegisterCallback(callback);
}
}  // namespace Sensors
}  // namespace OHOS
