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

#include "sensor_agent_type.h"
#include "sensor_data_channel.h"
#include "sensor_service_client.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_TEST, "SensorNativeTest" };
}

class SensorNativeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void HandleEvent(struct SensorEvent *events, int32_t num, void *data);
    std::vector<Sensor> sensorsList_;
    uint32_t sensorId_;
    sptr<SensorDataChannel> sensorDataChannel_;
    std::unique_ptr<SensorServiceClient> sensorServiceClient_;
    bool dataReport_;
};
void SensorNativeTest::HandleEvent(struct SensorEvent *events, int32_t num, void *data)
{
    ASSERT_NE(events, nullptr);
    HiLog::Info(LABEL, "%{public}s start,num : %{public}d", __func__, num);
    SensorNativeTest *test = reinterpret_cast<SensorNativeTest *>(data);
    ASSERT_NE(test, nullptr);
    for (int32_t i = 0; i < num; i++) {
        if (events[i].sensorTypeId == (int32_t)test->sensorId_) {
            test->dataReport_ = true;
        }
    }
    return;
}
void SensorNativeTest::SetUpTestCase()
{}

void SensorNativeTest::TearDownTestCase()
{}

void SensorNativeTest::SetUp()
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    sensorDataChannel_ = new (std::nothrow) SensorDataChannel();
    ASSERT_NE(sensorDataChannel_, nullptr);
    sensorServiceClient_ = std::make_unique<SensorServiceClient>();
    ASSERT_NE(sensorServiceClient_, nullptr);
    sensorsList_ = sensorServiceClient_->GetSensorList();
    ASSERT_NE(sensorsList_.size(), 0UL);
    sensorId_ = sensorsList_[0].GetSensorId();
    auto ret = sensorDataChannel_->CreateSensorDataChannel(HandleEvent, this);
    HiLog::Info(LABEL, "CreateSensorDataChannel ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    sensorServiceClient_->TransferDataChannel(sensorDataChannel_);
    dataReport_ = false;
}

void SensorNativeTest::TearDown()
{
    sensorServiceClient_->DestroyDataChannel();
    sensorDataChannel_->DestroySensorDataChannel();
}

/*
 * @tc.name: SensorOperation_001
 * @tc.desc: enable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, SensorOperation_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "SensorOperation_001 begin");
    uint64_t samplingPeriodNs = 10000000;
    uint64_t maxReportDelayNs = 0;
    auto sensorId = sensorsList_[0].GetSensorId();
    auto ret = sensorServiceClient_->EnableSensor(sensorId, samplingPeriodNs, maxReportDelayNs);
    HiLog::Info(LABEL, "EnableSensor ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    dataReport_ = false;
    // wait evennt
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ASSERT_EQ(dataReport_, true);
}
}  // namespace Sensors
}  // namespace OHOS
