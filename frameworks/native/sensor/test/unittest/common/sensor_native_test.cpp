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
    int32_t ret = sensorDataChannel_->CreateSensorDataChannel(HandleEvent, this);
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
 * @tc.name: GetSensorList_001
 * @tc.desc: get sensor list
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, GetSensorList_001, TestSize.Level0)
{
    bool ret = sensorsList_.empty();
    HiLog::Info(LABEL, "GetSensorList_001,count : %{public}d!", int{ sensorsList_.size() });
    ASSERT_EQ(ret, false);
}

/*
 * @tc.name: GetSensorList_002
 * @tc.desc: Judge sensor info
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, GetSensorList_002, TestSize.Level0)
{
    for (const auto &it : sensorsList_) {
        ASSERT_EQ(it.GetSensorId() == 0, true);
        ASSERT_EQ(it.GetName().empty(), false);
        ASSERT_EQ(it.GetVendor().empty(), false);
    }
    HiLog::Info(LABEL, "GetSensorList_002");
}

/*
 * @tc.name: ClientSensorDataChannel_001
 * @tc.desc: do client sensor send data channel close
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, ClientSensorDataChannel_001, TestSize.Level0)
{
    int32_t ret = sensorDataChannel_->GetSendDataFd();
    HiLog::Info(LABEL, "ClientSensorDataChannel_001,ret : %{public}d!", ret);
    ASSERT_EQ(ret, -1);
}

/*
 * @tc.name: ClientSensorDataChannel_002
 * @tc.desc: judge channel fd when create channel
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, ClientSensorDataChannel_002, TestSize.Level0)
{
    int32_t ret = sensorDataChannel_->GetReceiveDataFd();
    ASSERT_EQ((ret >= 0), true);
}

/*
 * @tc.name: ClientSensorDataChannel_003
 * @tc.desc: Judge read thread status when Destroy Channel
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, ClientSensorDataChannel_003, TestSize.Level0)
{
    int32_t ret = sensorDataChannel_->DestroySensorDataChannel();
    ASSERT_EQ(ret, ERR_OK);
    bool result = false;
    result = sensorDataChannel_->IsThreadExit();
    ASSERT_EQ(result, true);
}

/*
 * @tc.name: ClientSensorDataChannel_004
 * @tc.desc: Destroy ClientSensorDataChannel
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, ClientSensorDataChannel_004, TestSize.Level0)
{
    int32_t ret = sensorDataChannel_->DestroySensorDataChannel();
    ASSERT_EQ(ret, ERR_OK);
    ret = sensorDataChannel_->GetSendDataFd();
    ASSERT_EQ(ret, -1);
    ret = sensorDataChannel_->GetReceiveDataFd();
    ASSERT_EQ(ret, -1);
}

/*
 * @tc.name: SensorOperation_001
 * @tc.desc: disable sensor
 * @tc.type: FUNC
 */
HWTEST_F(SensorNativeTest, SensorOperation_001, TestSize.Level1)
{
    auto sensorId = sensorsList_[0].GetSensorId();
    auto ret = sensorServiceClient_->DisableSensor(sensorId);
    HiLog::Info(LABEL, "DisableSensor ret is : %{public}d", ret);
    ASSERT_EQ(ret, ERR_OK);
    dataReport_ = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(dataReport_, false);
}
}  // namespace Sensors
}  // namespace OHOS
