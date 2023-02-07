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

#include "sensor_power_test.h"

#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "sensor_agent.h"
#include "sensor_status_callback.h"
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
constexpr int32_t sensorId {1};
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
int32_t g_callback_pid = 0;
int32_t g_callback_sensorId = 0;
bool g_callback_isActive = false;
int64_t g_callback_samplingPeriodNs = 0;
int64_t g_callback_maxReportDelayNs = 0;

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
    int res = SetSelfTokenID(tokenId);
    ASSERT_EQ(res, 0);
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

HWTEST_F(SensorPowerTest, SuspendSensorsTest_001, TestSize.Level1)
{
    SEN_HILOGI("SuspendSensorsTest_001 in");
    int32_t ret = SuspendSensors(invalidValue);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, ResumeSensorsTest_001, TestSize.Level1)
{
    SEN_HILOGI("ResumeSensorsTest_001 in");
    int32_t ret = ResumeSensors(invalidValue);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, GetAppSensorsTest_001, TestSize.Level1)
{
    SEN_HILOGI("GetAppSensorsTest_001 in");
    AppSensorInfo *appSensorInfos {nullptr};
    int32_t count { 0 };
    int32_t ret = GetAppSensors(invalidValue, &appSensorInfos, &count);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, RegisterCallbackTest_001, TestSize.Level1)
{
    SEN_HILOGI("RegisterCallbackTest_001 in");
    sptr<ISensorStatusCallback> sensorCallback = nullptr;
    int32_t ret = RegisterCallback(sensorCallback);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_001, TestSize.Level1)
{
    SEN_HILOGI("SensorPowerTest_001 in");
    sptr<ISensorStatusCallback> sensorCallback = new (std::nothrow) SensorStatusCallback();
    int32_t ret = RegisterCallback(sensorCallback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (g_callback_pid == process_pid) {
        SEN_HILOGI("Callback: pid:%{public}d, sensorId:%{public}d, isActive:%{public}d,\
                    samplingPeriodNs:%{public}lld, maxReportDelayNs:%{public}lld",
                    g_callback_pid, g_callback_sensorId, g_callback_isActive,
                    g_callback_samplingPeriodNs, g_callback_maxReportDelayNs);
    }

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_002, TestSize.Level1)
{
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

    AppSensorInfo *appSensorInfos {nullptr};
    int32_t count { 0 };
    ret = GetAppSensors(process_pid, &appSensorInfos, &count);
    for (int32_t i = 0; i < count; ++i) {
        AppSensorInfo *curAppSensor = appSensorInfos + i;
        SEN_HILOGI("i:%{public}d, pid:%{public}d, sensorId:%{public}d, isActive:%{public}d,\
                    samplingPeriodNs:%{public}lld, maxReportDelayNs:%{public}lld",
                    i, curAppSensor->pid, curAppSensor->sensorId, curAppSensor->isActive,
                    curAppSensor->samplingPeriodNs, curAppSensor->maxReportDelayNs);
    }
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_003, TestSize.Level1)
{
    SEN_HILOGI("SensorPowerTest_003 in");
    sptr<ISensorStatusCallback> sensorCallback = new (std::nothrow) SensorStatusCallback();
    int32_t ret = RegisterCallback(sensorCallback);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (g_callback_pid == process_pid) {
        SEN_HILOGI("Callback: pid:%{public}d, sensorId:%{public}d, isActive:%{public}d,\
                    samplingPeriodNs:%{public}lld, maxReportDelayNs:%{public}lld",
                    g_callback_pid, g_callback_sensorId, g_callback_isActive,
                    g_callback_samplingPeriodNs, g_callback_maxReportDelayNs);
        ret = SuspendSensors(g_callback_pid);
        ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ret = ResumeSensors(g_callback_pid);
        ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    ret = DeactivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(SensorPowerTest, SensorPowerTest_004, TestSize.Level1)
{
    SEN_HILOGI("SensorPowerTest_004 in");
    SensorUser user;
    user.callback = SensorDataCallbackImpl;
    int32_t ret = SubscribeSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(sensorId, &user, 100000000, 100000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(sensorId, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    AppSensorInfo *appSensorInfos {nullptr};
    int32_t count { 0 };
    ret = GetAppSensors(process_pid, &appSensorInfos, &count);
    for (int32_t i = 0; i < count; ++i) {
        AppSensorInfo *curAppSensor = appSensorInfos + i;
        SEN_HILOGI("i:%{public}d, pid:%{public}d, sensorId:%{public}d, isActive:%{public}d,\
                    samplingPeriodNs:%{public}lld, maxReportDelayNs:%{public}lld",
                    i, curAppSensor->pid, curAppSensor->sensorId, curAppSensor->isActive,
                    curAppSensor->samplingPeriodNs, curAppSensor->maxReportDelayNs);
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
}  // namespace Sensors
}  // namespace OHOS
