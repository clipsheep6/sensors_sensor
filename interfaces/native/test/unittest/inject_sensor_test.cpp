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

#include <atomic>
#include <cinttypes>
#include <thread>

#include <gtest/gtest.h>

#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_errors.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, OHOS::Sensors::SENSOR_LOG_DOMAIN, "InjectSensorTest" };
constexpr float ANGLE_MAX = 180.0F;
constexpr float ANGLE_MIN = 0.0F;
}  // namespace

class InjectSensorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void InjectSensorTest::SetUpTestCase(){}

void InjectSensorTest::TearDownTestCase() {}

void InjectSensorTest::SetUp() {}

void InjectSensorTest::TearDown() {}

void PostureDataCallbackImpl(SensorEvent *event)
{
    if (event == nullptr) {
        SEN_HILOGE("event is nullptr");
        return;
    }
    if (event[0].data == nullptr) {
        SEN_HILOGE("event[0].data is nullptr");
        return;
    }
    if (event[0].dataLen < sizeof(PostureData)) {
        SEN_HILOGE("Event dataLen less than posture data size, event.dataLen:%{public}u", event[0].dataLen);
        return;
    }
    PostureData *postureData = reinterpret_cast<PostureData *>(event[0].data);
    float angle = postureData->angle;
    if ((angle < ANGLE_MIN && std::fabs(angle - ANGLE_MIN) > std::numeric_limits<float>::epsilon())
        || (angle > ANGLE_MAX && std::fabs(angle - ANGLE_MAX) > std::numeric_limits<float>::epsilon())) {
        SEN_HILOGE("Invalid posture angle, angle:%{public}f", angle);
        return;
    }
    SEN_HILOGI("sensorId:%{public}d, version:%{public}d, dataLen:%{public}u, gxm:%{public}f, "
        "gym:%{public}f, gzm:%{public}f, gxs:%{public}f, gys:%{public}f, gzs:%{public}f, angle:%{public}f",
        event[0].sensorTypeId, event[0].version, event[0].dataLen, postureData->gxm, postureData->gym,
        postureData->gzm, postureData->gxs, postureData->gys, postureData->gzs, postureData->angle);
}

HWTEST_F(InjectSensorTest, InjectSensorTest_001, TestSize.Level1)
{
    SEN_HILOGI("InjectSensorTest_001 enter");
    int32_t ret = InjectMockSensor(-1);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(InjectSensorTest, InjectSensorTest_002, TestSize.Level1)
{
    SEN_HILOGI("InjectSensorTest_002 enter");
    int32_t ret = UninjectMockSensor(-1);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(InjectSensorTest, InjectSensorTest_003, TestSize.Level1)
{
    SEN_HILOGI("InjectSensorTest_003 enter");
    int32_t ret = InjectMockSensor(1);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(InjectSensorTest, InjectSensorTest_004, TestSize.Level1)
{
    SEN_HILOGI("InjectSensorTest_004 enter");
    int32_t ret = UninjectMockSensor(-1);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}

HWTEST_F(InjectSensorTest, InjectSensorTest_005, TestSize.Level1)
{
    SEN_HILOGI("InjectSensorTest_005 enter");
    //查询sensorList
    SensorInfo *sensorInfo1 = nullptr;
    int32_t count1 = 0;
    int32_t ret = GetAllSensors(&sensorInfo1, &count1);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    SEN_HILOGI("ZMH, 1, SensorList size:%{public}d", count1);
    for (int32_t i = 0; i < count1; ++i) {
        SEN_HILOGI("ZMH, 1, sensorName:%{public}s, sensorId:%{public}d, minSamplePeriod:%{public}" PRId64
            " ns, maxSamplePeriod:%{public}" PRId64 " ns", sensorInfo1[i].sensorName, sensorInfo1[i].sensorId,
            sensorInfo1[i].minSamplePeriod, sensorInfo1[i].maxSamplePeriod);
    }

    //注入姿态传感器
    ret = InjectMockSensor(SENSOR_TYPE_ID_POSTURE);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    //查询sensorList
    SensorInfo *sensorInfo2 = nullptr;
    int32_t count2 = 0;
    ret = GetAllSensors(&sensorInfo2, &count2);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    SEN_HILOGI("ZMH, 2, SensorList size:%{public}d", count2);
    for (int32_t i = 0; i < count2; ++i) {
        SEN_HILOGI("ZMH, 2, sensorName:%{public}s, sensorId:%{public}d, minSamplePeriod:%{public}" PRId64
            " ns, maxSamplePeriod:%{public}" PRId64 " ns", sensorInfo2[i].sensorName, sensorInfo2[i].sensorId,
            sensorInfo2[i].minSamplePeriod, sensorInfo2[i].maxSamplePeriod);
    }

    //订阅姿态传感器
    SensorUser user;
    user.callback = PostureDataCallbackImpl;
    ret = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = SetBatch(SENSOR_TYPE_ID_POSTURE, &user, 10000000, 10000000);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    //取消订阅姿态传感器
    ret = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    ret = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    //删除注入姿态传感器
    ret =  UninjectMockSensor(SENSOR_TYPE_ID_POSTURE);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);

    //查询sensorList
    SensorInfo *sensorInfo3 = nullptr;
    int32_t count3 = 0;
    ret = GetAllSensors(&sensorInfo3, &count3);
    ASSERT_EQ(ret, OHOS::Sensors::SUCCESS);
    SEN_HILOGI("ZMH, 3, SensorList size:%{public}d", count3);
    for (int32_t i = 0; i < count3; ++i) {
        SEN_HILOGI("ZMH, 3, sensorName:%{public}s, sensorId:%{public}d, minSamplePeriod:%{public}" PRId64
            " ns, maxSamplePeriod:%{public}" PRId64 " ns", sensorInfo3[i].sensorName, sensorInfo3[i].sensorId,
            sensorInfo3[i].minSamplePeriod, sensorInfo3[i].maxSamplePeriod);
    }
    //尝试订阅姿态传感器
    ret = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    ASSERT_NE(ret, OHOS::Sensors::SUCCESS);
}
}  // namespace Sensors
}  // namespace OHOS
