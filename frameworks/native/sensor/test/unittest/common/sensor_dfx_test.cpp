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
#include <memory>
#include <thread>
#include <unistd.h>

#include <fcntl.h>

#include <gtest/gtest.h>
#include "sensor_data_channel.h"
#include "sensor_data_event.h"
#include "sensor_service_client.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"
#include "string_ex.h"


namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_TEST, "SensorDFXTest" };
const std::string CMD_LINE = "ps -ef | grep 'hsensors' | grep -v grep | awk '{print $2}'";
constexpr int32_t BUFFER_SIZE = 8;
constexpr pid_t INVALID_PID = -1;
}  // namespace

class SensorDFXTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    pid_t GetSensorServicePid();
    static void HandleEvent(struct SensorEvent *events, int32_t num, void *data);
    static std::vector<Sensor> g_sensorsList;
    static sptr<SensorDataChannel> g_sensorDataChannel;
    static std::unique_ptr<SensorServiceClient> g_sensorServiceClient;
    static bool g_dataReport;
};

std::vector<Sensor> SensorDFXTest::g_sensorsList;
sptr<SensorDataChannel> SensorDFXTest::g_sensorDataChannel = nullptr;
bool SensorDFXTest::g_dataReport = false;
std::unique_ptr<SensorServiceClient> SensorDFXTest::g_sensorServiceClient = nullptr;

pid_t SensorDFXTest::GetSensorServicePid()
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

void SensorDFXTest::HandleEvent(struct SensorEvent *events, int32_t num, void *data)
{
    HiLog::Info(LABEL, "%{public}s HandleEvent", __func__);
    for (int32_t i = 0; i < num; i++) {
        g_dataReport = true;
    }
    return;
}

void SensorDFXTest::SetUpTestCase()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    g_sensorDataChannel = new (std::nothrow) SensorDataChannel();
    ASSERT_NE(g_sensorDataChannel, nullptr);
    g_sensorServiceClient = std::make_unique<SensorServiceClient>();
    ASSERT_NE(g_sensorServiceClient, nullptr);
    g_sensorsList = g_sensorServiceClient->GetSensorList();
    ASSERT_NE(g_sensorsList.size(), 0UL);
    auto ret = g_sensorDataChannel->CreateSensorDataChannel(HandleEvent, nullptr);
    HiLog::Info(LABEL, "CreateSensorDataChannel ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    ret = g_sensorServiceClient->TransferDataChannel(g_sensorDataChannel);
    HiLog::Info(LABEL, "TransferDataChannel ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    g_dataReport = false;
}

void SensorDFXTest::TearDownTestCase()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    g_sensorServiceClient->DestroyDataChannel();
    g_sensorDataChannel->DestroySensorDataChannel();
}

void SensorDFXTest::SetUp()
{}

void SensorDFXTest::TearDown()
{}
}  // namespace Sensors
}  // namespace OHOS
