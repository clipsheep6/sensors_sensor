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
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "report_data_callback.h"
#include "sensor_data_channel.h"
#include "sensor_service_client.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_TEST, "SensorDataProcesserTest" };
constexpr uint32_t ACC_SENSOR_ID = 0;
constexpr uint32_t WAIT_TIME = 3000;
constexpr uint64_t SAMPLING_PEROID_NS = 200000000;
constexpr uint64_t MAX_REPORT_DELAY_NS = 0;
constexpr uint32_t STEP_COUNTER_SENSORID = 0;
constexpr uint32_t STEP_DETECTOR_SENSORID = 0;
constexpr uint32_t OTHER = 4;
constexpr uint32_t SENSOR_TYPE_FLUSH = 4;
constexpr uint32_t FIRST_INDEX = 1;
constexpr uint32_t SENSOR_INDEX_SHIFT = 8;
constexpr uint32_t SENSOR_TYPE_SHIFT = 16;
constexpr uint32_t SENSOR_CATAGORY_SHIFT = 24;

constexpr uint32_t FLUSH_COMPLETE_ID = (static_cast<uint32_t>(OTHER) << SENSOR_CATAGORY_SHIFT) |
                                       (static_cast<uint32_t>(SENSOR_TYPE_FLUSH) << SENSOR_TYPE_SHIFT) |
                                       (static_cast<uint32_t>(FIRST_INDEX) << SENSOR_INDEX_SHIFT);
class SensorDataProcesserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void HandleEvent(struct SensorEvent *events, int32_t num, void *data);
    static std::vector<Sensor> g_sensorsList;
    static sptr<SensorDataChannel> g_dataChannel;
    static sptr<ReportDataCallback> g_dataCallback;
    static std::vector<struct SensorEvent> g_eventsBuf;
    static std::unique_ptr<SensorServiceClient> g_serviceClient;
    static int32_t g_dataCount;
    static bool g_getFlushComplete;
};

struct SensorEvent g_event;
int32_t SensorDataProcesserTest::g_dataCount = 0;
bool SensorDataProcesserTest::g_getFlushComplete = false;
std::vector<Sensor> SensorDataProcesserTest::g_sensorsList;
sptr<SensorDataChannel> SensorDataProcesserTest::g_dataChannel;
sptr<ReportDataCallback> SensorDataProcesserTest::g_dataCallback;
std::vector<struct SensorEvent> SensorDataProcesserTest::g_eventsBuf;
std::unique_ptr<SensorServiceClient> SensorDataProcesserTest::g_serviceClient;
}  // namespace

void SensorDataProcesserTest::HandleEvent(struct SensorEvent *events, int32_t num, void *data)
{
    HiLog::Info(LABEL, "HandleEvent begin");
    if (num <= 0) {
        HiLog::Error(LABEL, "%{public}s failed, num : %{public}d", __func__, num);
        return;
    }
    g_event.sensorTypeId = events[0].sensorTypeId;
    for (int32_t i = 0; i < num; i++) {
        if (events[i].sensorTypeId == ACC_SENSOR_ID) {
            g_dataCount++;
        }
        if (events[i].sensorTypeId == FLUSH_COMPLETE_ID) {
            g_getFlushComplete = true;
        }
    }
}

void SensorDataProcesserTest::SetUpTestCase()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    g_dataChannel = new (std::nothrow) SensorDataChannel();
    ASSERT_NE(g_dataChannel, nullptr);
    g_serviceClient = std::make_unique<SensorServiceClient>();
    ASSERT_NE(g_serviceClient, nullptr);
    g_dataCallback = new (std::nothrow) ReportDataCallback();
    ASSERT_NE(g_dataCallback, nullptr);
    g_sensorsList = g_serviceClient->GetSensorList();
    auto ret = g_dataChannel->CreateSensorDataChannel(HandleEvent, nullptr);
    HiLog::Info(LABEL, "CreateSensorDataChannel ret : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    g_serviceClient->TransferDataChannel(g_dataChannel);
}

void SensorDataProcesserTest::TearDownTestCase()
{
    g_dataChannel->DestroySensorDataChannel();
    g_serviceClient->DestroyDataChannel();
}

void SensorDataProcesserTest::SetUp()
{}

void SensorDataProcesserTest::TearDown()
{}

/*
 * @tc.name: EnableSensor_001
 * @tc.desc: enable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, EnableSensor_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "EnableSensor_001 begin");
    uint64_t samplingPeroidNs = SAMPLING_PEROID_NS;
    uint64_t maxReportDelayNs = MAX_REPORT_DELAY_NS;
    uint32_t sensorId = ACC_SENSOR_ID;
    auto ret = g_serviceClient->EnableSensor(sensorId, samplingPeroidNs, maxReportDelayNs);
    HiLog::Info(LABEL, "EnableSensor ret : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    HiLog::Info(LABEL, "EnableSensor_001 end");
}

/*
 * @tc.name: ReadSensorData_001
 * @tc.desc: read sensor data
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, ReadSensorData_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "ReadSensorData_001 begin");
    g_dataCount = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
    HiLog::Info(LABEL, "ReadSensorData_001 end");
}

/*
 * @tc.name: ReportData_001
 * @tc.desc: judge whether the data is valid
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, ReportData_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "ReportData_001 begin");
    ASSERT_EQ(g_event.sensorTypeId, 0);
    HiLog::Info(LABEL, "ReportData_001 end, %{public}d", g_event.sensorTypeId);
}

/*
 * @tc.name: EnableStepCounter_001
 * @tc.desc: enable step counter sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, EnableStepCounter_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "EnableStepCounter begin");
    ASSERT_NE(g_serviceClient, nullptr);
    const int64_t samplingPeriodNs = 50000000;
    const int64_t maxReportDelayNs = 0;
    auto ret = g_serviceClient->EnableSensor(STEP_COUNTER_SENSORID, samplingPeriodNs, maxReportDelayNs);
    HiLog::Info(LABEL, "Enable step counter, ret : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: DisableStepCounter_001
 * @tc.desc: disable step counter sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, DisableStepCounter_001, TestSize.Level1)
{
    ASSERT_NE(g_serviceClient, nullptr);
    auto ret = g_serviceClient->DisableSensor(STEP_COUNTER_SENSORID);
    HiLog::Info(LABEL, "Disable step counter, ret : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: DisableStepDetector_001
 * @tc.desc: disable step detector sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorDataProcesserTest, DisableStepDetector_001, TestSize.Level1)
{
    ASSERT_NE(g_serviceClient, nullptr);
    auto ret = g_serviceClient->DisableSensor(STEP_DETECTOR_SENSORID);
    HiLog::Info(LABEL, "Disable step detector ret : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
}
}  // namespace Sensors
}  // namespace OHOS
