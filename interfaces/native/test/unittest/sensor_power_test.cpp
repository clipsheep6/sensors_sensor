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

#include <cinttypes>
#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "sensor_agent.h"
#include "sensors_errors.h"
#include "system_info.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, OHOS::Sensors::SENSOR_LOG_DOMAIN, "SensorPowerTest" };
constexpr int32_t sensorId { 1 };
constexpr int32_t invalidValue { -1 };
}  // namespace

class SensorPowerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

int32_t process_pid = 0;

void SensorPowerTest::SetUpTestCase()
{
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.ACCELEROMETER";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "SensorPowerTest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    ASSERT_EQ(SetSelfTokenID(tokenId), 0);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;

    SYSTEM_INFO::CpuInfo cpuInfo;
    const std::string process_name = "SensorPowerTest";
    process_pid = cpuInfo.GetTaskPidFile(process_name);
    SEN_HILOGI("Current process pid is %{public}d", process_pid);
    ASSERT_NE(process_pid, 0);
}

void SensorPowerTest::TearDownTestCase() {}

void SensorPowerTest::SetUp() {}

void SensorPowerTest::TearDown() {}

void SensorDataCallbackImpl(SensorEvent *event)
{
    if (event == nullptr) {
        SEN_HILOGE("SensorEvent is null");
        return;
    }
    float *sensorData = (float *)event[0].data;
    SEN_HILOGI("SensorId:%{public}d, version:%{public}d,dataLen:%{public}d,data:%{public}f",
        event[0].sensorTypeId, event[0].version, event[0].dataLen, *(sensorData));
}

void ClientInfoCallbackImpl(SubscribeSensorInfo &subscribeSensorInfo)
{
    SEN_HILOGI("pid:%{public}d, sensorId:%{public}d, isActive:%{public}d, "
               "samplingPeriodNs:%{public}" PRId64 ", maxReportDelayNs:%{public}" PRId64 "",
               subscribeSensorInfo.pid, subscribeSensorInfo.sensorId, subscribeSensorInfo.isActive,
               subscribeSensorInfo.samplingPeriodNs, subscribeSensorInfo.maxReportDelayNs);
}

void ClientInfoCallbackImpl2(SubscribeSensorInfo &subscribeSensorInfo)
{
    SEN_HILOGI("pid:%{public}d, sensorId:%{public}d, isActive:%{public}d, "
               "samplingPeriodNs:%{public}" PRId64 ", maxReportDelayNs:%{public}" PRId64 "",
               subscribeSensorInfo.pid, subscribeSensorInfo.sensorId, subscribeSensorInfo.isActive,
               subscribeSensorInfo.samplingPeriodNs, subscribeSensorInfo.maxReportDelayNs);
}

HWTEST_F(SensorPowerTest, SuspendSensorsTest_001, TestSize.Level1)
{
    // 休眠接口异常用例
    SEN_HILOGI("SuspendSensorsTest_001 in");
    int32_t ret = SuspendSensors(invalidValue);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, ResumeSensorsTest_001, TestSize.Level1)
{
    // 恢复接口异常用例
    SEN_HILOGI("ResumeSensorsTest_001 in");
    int32_t ret = ResumeSensors(invalidValue);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, GetSubscribeInfosTest_001, TestSize.Level1)
{
    // 查询接口异常用例
    SEN_HILOGI("GetSubscribeInfosTest_001 in");
    SubscribeSensorInfo *subscribeSensorInfo {nullptr};
    int32_t count { 0 };
    int32_t ret = GetSubscribeInfos(invalidValue, &subscribeSensorInfo, &count);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, RegisterClientInfoCallback_001, TestSize.Level1)
{
    // 注册接口异常用例
    SEN_HILOGI("RegisterClientInfoCallback_001 in");
    ClientInfoCallback callback = nullptr;
    int32_t ret = RegisterClientInfoCallback(callback);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, UnregisterClientInfoCallback_001, TestSize.Level1)
{
    // 取消注册接口异常用例
    SEN_HILOGI("UnregisterClientInfoCallback_001 in");
    ClientInfoCallback callback = nullptr;
    int32_t ret = UnregisterClientInfoCallback(callback);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_001, TestSize.Level1)
{
    // 场景用例1，订阅ACC传感器》查询传感器订阅数据》休眠进程的传感器》恢复进程的传感器》取消订阅ACC传感器
    SEN_HILOGI("SensorPowerTest_001 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;

    int32_t ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    SubscribeSensorInfo *subscribeSensorInfo {nullptr};
    int32_t count { 0 };
    ret = GetSubscribeInfos(process_pid, &subscribeSensorInfo, &count);
    for (int32_t i = 0; i < count; ++i) {
        SubscribeSensorInfo *curSubscribeSensorInfo = subscribeSensorInfo + i;
        SEN_HILOGI("i:%{public}d, pid:%{public}d, sensorId:%{public}d, isActive:%{public}d, "
                   "samplingPeriodNs:%{public}" PRId64 ", maxReportDelayNs:%{public}" PRId64 "",
                    i, curSubscribeSensorInfo->pid, curSubscribeSensorInfo->sensorId, curSubscribeSensorInfo->isActive,
                    curSubscribeSensorInfo->samplingPeriodNs, curSubscribeSensorInfo->maxReportDelayNs);
    }
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_002, TestSize.Level1)
{
    // 场景用例2，订阅ACC传感器》休眠进程的传感器》查询传感器订阅数据》恢复进程的传感器》取消订阅ACC传感器
    SEN_HILOGI("SensorPowerTest_002 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;

    int32_t ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    SubscribeSensorInfo *subscribeSensorInfo {nullptr};
    int32_t count { 0 };
    ret = GetSubscribeInfos(process_pid, &subscribeSensorInfo, &count);
    for (int32_t i = 0; i < count; ++i) {
        SubscribeSensorInfo *curSubscribeSensorInfo = subscribeSensorInfo + i;
        SEN_HILOGI("i:%{public}d, pid:%{public}d, sensorId:%{public}d, isActive:%{public}d, "
                   "samplingPeriodNs:%{public}" PRId64 ", maxReportDelayNs:%{public}" PRId64 "",
                    i, curSubscribeSensorInfo->pid, curSubscribeSensorInfo->sensorId, curSubscribeSensorInfo->isActive,
                    curSubscribeSensorInfo->samplingPeriodNs, curSubscribeSensorInfo->maxReportDelayNs);
    }
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_003, TestSize.Level1)
{
    // 场景用例3，订阅ACC传感器》休眠进程的传感器》恢复进程的传感器》查询传感器订阅数据》取消订阅ACC传感器
    SEN_HILOGI("SensorPowerTest_003 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;

    int32_t ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    SubscribeSensorInfo *subscribeSensorInfo {nullptr};
    int32_t count { 0 };
    ret = GetSubscribeInfos(process_pid, &subscribeSensorInfo, &count);
    for (int32_t i = 0; i < count; ++i) {
        SubscribeSensorInfo *curSubscribeSensorInfo = subscribeSensorInfo + i;
        SEN_HILOGI("i:%{public}d, pid:%{public}d, sensorId:%{public}d, isActive:%{public}d, "
                   "samplingPeriodNs:%{public}" PRId64 ", maxReportDelayNs:%{public}" PRId64 "",
                    i, curSubscribeSensorInfo->pid, curSubscribeSensorInfo->sensorId, curSubscribeSensorInfo->isActive,
                    curSubscribeSensorInfo->samplingPeriodNs, curSubscribeSensorInfo->maxReportDelayNs);
    }
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_004, TestSize.Level1)
{
    // 场景用例4，注册回调函数》订阅ACC传感器》休眠进程的传感器》恢复进程的传感器》取消订阅ACC传感器》取消注册回调函数
    SEN_HILOGI("SensorPowerTest_004 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    ClientInfoCallback callback = ClientInfoCallbackImpl;

    int32_t ret = RegisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = UnregisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_005, TestSize.Level1)
{
    // 场景用例5，注册回调函数》订阅ACC传感器》休眠进程的传感器》恢复进程的传感器》取消注册回调函数》取消订阅ACC传感器
    SEN_HILOGI("SensorPowerTest_005 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    ClientInfoCallback callback = ClientInfoCallbackImpl;

    int32_t ret = RegisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = UnregisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_006, TestSize.Level1)
{
    // 场景用例6，注册回调函数1》注册回调函数2》订阅ACC传感器》休眠进程的传感器》恢复进程的传感器》取消订阅ACC传感器》取消注册回调函数1》取消注册回调函数2
    SEN_HILOGI("SensorPowerTest_006 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    ClientInfoCallback callback = ClientInfoCallbackImpl;
    ClientInfoCallback callback2 = ClientInfoCallbackImpl2;

    int32_t ret = RegisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = RegisterClientInfoCallback(callback2);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = SuspendSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    ret = ResumeSensors(process_pid);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = UnregisterClientInfoCallback(callback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnregisterClientInfoCallback(callback2);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}
}  // namespace Sensors
}  // namespace OHOS