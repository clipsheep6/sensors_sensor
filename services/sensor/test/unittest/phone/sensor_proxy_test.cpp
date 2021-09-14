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

#include <cstdint>
#include <thread>
#include <unistd.h>
#include <vector>

#include <fcntl.h>

#include <gtest/gtest.h>
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sensor_data_event.h"
#include "sensor_service_proxy.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_TEST, "SensorProxyTest" };
const uint32_t INVALID_SENSOR_ID = -1;
const std::string CMD_LINE = "ps -ef | grep 'hardware.sensors' | grep -v grep | awk '{print $2}'";
constexpr int32_t BUFFER_SIZE = 8;
constexpr pid_t INVALID_PID = -1;

class SensorProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    pid_t GetSensorServicePid();
    sptr<ISensorService> sensorProxy_;
    std::vector<Sensor> sensors_;
    uint32_t sensorId_;
};
}  // namespace

void SensorProxyTest::SetUpTestCase()
{}

void SensorProxyTest::TearDownTestCase()
{}

void SensorProxyTest::SetUp()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(systemAbilityManager, nullptr);
    sensorProxy_ = iface_cast<ISensorService>(systemAbilityManager->GetSystemAbility(SENSOR_SERVICE_ABILITY_ID));
    ASSERT_NE(sensorProxy_, nullptr);
    sensors_ = sensorProxy_->GetSensorList();
    ASSERT_NE(sensors_.size(), 0UL);
    sensorId_ = sensors_[0].GetSensorId();
    ASSERT_NE(sensorId_, INVALID_SENSOR_ID);
    HiLog::Info(LABEL, "%{public}s end", __func__);
}

void SensorProxyTest::TearDown()
{}

pid_t SensorProxyTest::GetSensorServicePid()
{
    pid_t pid = INVALID_PID;
    char buf[BUFFER_SIZE] = { 0 };
    FILE *fp = popen(CMD_LINE.c_str(), "r");
    if (fp == nullptr) {
        HiLog::Error(LABEL, "get error when getting sensor service process id");
        return pid;
    }

    fgets(buf, sizeof(buf) - 1, fp);
    pclose(fp);
    fp = nullptr;
    HiLog::Info(LABEL, "process is : %{public}s", buf);

    std::string pidStr(buf);
    pidStr = TrimStr(pidStr, '\n');
    HiLog::Info(LABEL, "pidStr is : %{public}s", pidStr.c_str());
    if (pidStr.empty()) {
        return pid;
    }

    if (IsNumericStr(pidStr)) {
        pid = std::stoi(pidStr);
    }
    return pid;
}

/*
 * @tc.name: EnableSensor_001
 * @tc.desc: enable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, EnableSensor_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "EnableSensor begin");
    ASSERT_NE(sensorProxy_, nullptr);
    int64_t samplingPeriodNs = 100000000L;
    int64_t maxReportDelayNs = 0L;
    auto ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    HiLog::Info(LABEL, "EnableSensor ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);

    samplingPeriodNs = 50000000L;
    maxReportDelayNs = 0L;
    ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    ASSERT_EQ(ret, ERR_OK);

    samplingPeriodNs = 20000000L;
    maxReportDelayNs = 0L;
    ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    ASSERT_EQ(ret, ERR_OK);
    samplingPeriodNs = 10000000L;
    maxReportDelayNs = 0L;
    ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    ASSERT_EQ(ret, ERR_OK);

    samplingPeriodNs = 20000000L;
    maxReportDelayNs = 0L;
    ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    ASSERT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: EnableSensor_002
 * @tc.desc: enable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, EnableSensor_002, TestSize.Level1)
{
    HiLog::Info(LABEL, "EnableSensor begin");
    ASSERT_NE(sensorProxy_, nullptr);
    const int64_t samplingPeriodNs = 50000000;
    const int64_t maxReportDelayNs = 0;
    auto ret = sensorProxy_->EnableSensor(sensorId_, samplingPeriodNs, maxReportDelayNs);
    HiLog::Info(LABEL, "EnableSensor ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: EnableSensor_003
 * @tc.desc: enable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, EnableSensor_003, TestSize.Level1)
{
    HiLog::Info(LABEL, "EnableSensor begin");
    ASSERT_NE(sensorProxy_, nullptr);
    const int64_t samplingPeriodNs = 50000000;
    const int64_t maxReportDelayNs = 0;
    auto ret = sensorProxy_->EnableSensor(INVALID_SENSOR_ID, samplingPeriodNs, maxReportDelayNs);
    HiLog::Info(LABEL, "EnableSensor ret is : %{public}d", ret);
    ASSERT_NE(ret, ERR_OK);
}

/*
 * @tc.name: RunCommand_001
 * @tc.desc: run command flush
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, RunCommand_001, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    uint32_t cmdType = 0;  // flush cmd
    uint32_t params = 1;    // flush cmd doesn't need this param, just give a value

    auto ret = sensorProxy_->RunCommand(sensorId_, cmdType, params);
    HiLog::Info(LABEL, "RunCommand_001 ret is : %{public}d", ret);
    // because the SensorProxyTest haven't calling TransferDataChannel, so RunCommand should return fail.
    ASSERT_NE(ret, ERR_OK);
}

/*
 * @tc.name: RunCommand_002
 * @tc.desc: run command unknown
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, RunCommand_002, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    uint32_t cmdType = 4;  // unknown cmd
    uint32_t params = 1;
    auto ret = sensorProxy_->RunCommand(sensorId_, cmdType, params);
    ASSERT_NE(ret, ERR_OK);
}

/*
 * @tc.name: RunCommand_003
 * @tc.desc: run command unknown
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, RunCommand_003, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    uint32_t cmdType = 4;  // unknown cmd
    uint32_t params = 1;
    auto ret = sensorProxy_->RunCommand(INVALID_SENSOR_ID, cmdType, params);
    ASSERT_NE(ret, ERR_OK);
}

/*
 * @tc.name: RunCommand_004
 * @tc.desc: run command unknown
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, RunCommand_004, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    uint32_t cmdType = 0;  // flush cmd
    uint32_t params = 1;
    auto ret = sensorProxy_->RunCommand(INVALID_SENSOR_ID, cmdType, params);
    ASSERT_NE(ret, ERR_OK);
}

/*
 * @tc.name: DisableSensor_001
 * @tc.desc: disable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, DisableSensor_001, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    auto ret = sensorProxy_->DisableSensor(sensorId_);
    HiLog::Info(LABEL, "DisableSensor ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: DisableSensor_002
 * @tc.desc: disable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorProxyTest, DisableSensor_002, TestSize.Level1)
{
    ASSERT_NE(sensorProxy_, nullptr);
    auto ret = sensorProxy_->DisableSensor(INVALID_SENSOR_ID);
    HiLog::Info(LABEL, "DisableSensor ret is : %{public}d", ret);
    ASSERT_NE(ret, ERR_OK);
}
}  // namespace Sensors
}  // namespace OHOS
