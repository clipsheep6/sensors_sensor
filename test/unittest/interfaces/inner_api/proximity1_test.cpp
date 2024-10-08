/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_errors.h"

#undef LOG_TAG
#define LOG_TAG "Proximity1Test"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
std::atomic_bool g_existProximity1 = false;
} // namespace

class Proximity1Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Proximity1Test::SetUpTestCase()
{
    SensorInfo *sensorInfo = nullptr;
    int32_t count = 0;
    int32_t ret = GetAllSensors(&sensorInfo, &count);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    if (sensorInfo == nullptr || count == 0) {
        SEN_HILOGE("sensorInfo is nullptr or count is 0");
        return;
    }
    for (int32_t i = 0; i < count; ++i) {
        if (sensorInfo[i].sensorId == SENSOR_TYPE_ID_PROXIMITY1) {
            g_existProximity1 = true;
            SEN_HILOGD("Exist ambient light sensor");
            break;
        }
    }
    SEN_HILOGD("Not exist ambient light sensor");
}

void Proximity1Test::TearDownTestCase() {}

void Proximity1Test::SetUp() {}

void Proximity1Test::TearDown() {}

void ProximityDataCallbackImpl(SensorEvent *event)
{
    if (event == nullptr) {
        SEN_HILOGE("event is nullptr");
        return;
    }
    if (event[0].data == nullptr) {
        SEN_HILOGE("event[0].data is nullptr");
        return;
    }
    if (event[0].dataLen < sizeof(ProximityData)) {
        SEN_HILOGE("Event dataLen less than proximity data size, event.dataLen:%{public}u", event[0].dataLen);
        return;
    }
    ProximityData *proximityData = reinterpret_cast<struct ProximityData *>(event[0].data);
    if (proximityData == nullptr) {
        SEN_HILOGE("proximityData is nullptr");
        return;
    }
    SEN_HILOGD("sensorId:%{public}d, version:%{public}d, dataLen:%{public}u, distance:%{public}f",
        event[0].sensorTypeId, event[0].version, event[0].dataLen, proximityData->distance);
}

void ProximityDataCallbackImpl2(SensorEvent *event)
{
    if (event == nullptr) {
        SEN_HILOGE("event is nullptr");
        return;
    }
    if (event[0].data == nullptr) {
        SEN_HILOGE("event[0].data is nullptr");
        return;
    }
    if (event[0].dataLen < sizeof(ProximityData)) {
        SEN_HILOGE("Event dataLen less than proximity data size, event.dataLen:%{public}u", event[0].dataLen);
        return;
    }
    ProximityData *proximityData = reinterpret_cast<struct ProximityData *>(event[0].data);
    if (proximityData == nullptr) {
        SEN_HILOGE("proximityData is nullptr");
        return;
    }
    SEN_HILOGD("sensorId:%{public}d, version:%{public}d, dataLen:%{public}u, distance:%{public}f",
        event[0].sensorTypeId, event[0].version, event[0].dataLen, proximityData->distance);
}

HWTEST_F(Proximity1Test, Proximity1Test_001, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_001 enter");
    if (g_existProximity1) {
        ASSERT_NE(ActivateSensor(SENSOR_TYPE_ID_PROXIMITY1, nullptr), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_002, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_002 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(ActivateSensor(-1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_003, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_003 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = nullptr;
        ASSERT_NE(ActivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_004, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_004 enter");
    if (g_existProximity1) {
        ASSERT_NE(DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY1, nullptr), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_005, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_005 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(DeactivateSensor(-1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_006, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_006 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = nullptr;
        ASSERT_NE(DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_007, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_007 enter");
    if (g_existProximity1) {
        ASSERT_NE(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, nullptr, 100000000, 100000000), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_008, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_008 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(SetBatch(-1, &user, 100000000, 100000000), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_009, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_009 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = nullptr;
        ASSERT_NE(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, &user, 100000000, 100000000), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_010, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_010 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, &user, -1, -1), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_011, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_011 enter");
    if (g_existProximity1) {
        ASSERT_NE(SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, nullptr), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_012, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_012 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(SubscribeSensor(-1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_013, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_013 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = nullptr;
        ASSERT_NE(SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_014, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_014 enter");
    if (g_existProximity1) {
        ASSERT_NE(UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, nullptr), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_015, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_015 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_NE(UnsubscribeSensor(-1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_016, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_016 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = nullptr;
        ASSERT_NE(UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_017, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_017 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_EQ(SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, &user, 100000000, 100000000), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(ActivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
    }
}

HWTEST_F(Proximity1Test, Proximity1Test_018, TestSize.Level1)
{
    SEN_HILOGI("Proximity1Test_018 enter");
    if (g_existProximity1) {
        SensorUser user;
        user.callback = ProximityDataCallbackImpl;
        ASSERT_EQ(SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, &user, 100000000, 100000000), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(ActivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user), OHOS::Sensors::SUCCESS);

        SensorUser user2;
        user2.callback = ProximityDataCallbackImpl2;
        ASSERT_EQ(SubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user2), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(SetBatch(SENSOR_TYPE_ID_PROXIMITY1, &user2, 200000000, 200000000), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(ActivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user2), OHOS::Sensors::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(DeactivateSensor(SENSOR_TYPE_ID_PROXIMITY1, &user2), OHOS::Sensors::SUCCESS);
        ASSERT_EQ(UnsubscribeSensor(SENSOR_TYPE_ID_PROXIMITY1, &user2), OHOS::Sensors::SUCCESS);
    }
}
} // namespace Sensors
} // namespace OHOS
