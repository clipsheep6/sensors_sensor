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

#include <cinttypes>
#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "sensor_agent.h"
#include "sensor_capi.h"
#include "sensor_errors.h"
#include "system_info.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, OHOS::Sensors::SENSOR_LOG_DOMAIN, "SensorCapiTest" };
// Sensor_SubscribeId SENSOR_ID = 1;
// Sensor_SubscribeId INVALID_VALUE = -1;

PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "ohos.permission.ACCELEROMETER",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test sensor capi",
    .descriptionId = 1,
    .availableLevel = APL_NORMAL
};

PermissionStateFull g_infoManagerTestState = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.ACCELEROMETER",
    .resDeviceID = {"local"}
};

HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {g_infoManagerTestPermDef},
    .permStateList = {g_infoManagerTestState}
};

HapInfoParams g_infoManagerTestInfoParms = {
    .bundleName = "sensorcapi_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "sensorCapiTest"
};
}  // namespace

class SensorCapiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
private:
    static AccessTokenID tokenID_;
};

AccessTokenID SensorCapiTest::tokenID_ = 0;

void SensorCapiTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
}

void SensorCapiTest::TearDownTestCase()
{
    int32_t ret = AccessTokenKit::DeleteToken(tokenID_);
    if (tokenID_ != 0) {
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

void SensorCapiTest::SetUp()
{}

void SensorCapiTest::TearDown()
{}

void SensorDataCallbackImpl(Sensor_SensorEvent *event)
{
    if (event == nullptr) {
        SEN_HILOGE("Sensor_SensorEvent is null");
        return;
    }
    float *sensorData = (float *)event[0].data;
    SEN_HILOGI("sensorId:%{public}d, version:%{public}d, dataLen:%{public}d, data:%{public}f",
        event[0].sensorTypeId, event[0].version, event[0].dataLen, *(sensorData));
}

HWTEST_F(SensorCapiTest, OH_Sensor_GetAllSensorsTest_001, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_GetAllSensors_001 in");
    Sensor_SensorInfo *sensorInfos { nullptr };
    int32_t count { 0 };
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_GetAllSensors(&sensorInfos, &count));
    ASSERT_EQ(ret, 0);
    ASSERT_NE(count, 0);
}

HWTEST_F(SensorCapiTest, OH_Sensor_GetAllSensorsTest_002, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_GetAllSensors_002 in");
    int32_t count = 0;
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_GetAllSensors(nullptr, &count));
    ASSERT_NE(ret, SENSOR_SUCCESS);
    ASSERT_EQ(count, 0);
}

HWTEST_F(SensorCapiTest, OH_Sensor_GetAllSensorsTest_003, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_GetAllSensors_003 in");
    Sensor_SensorInfo *sensorInfos { nullptr };
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_GetAllSensors(&sensorInfos, nullptr));
    ASSERT_NE(ret, SENSOR_SUCCESS);
}

HWTEST_F(SensorCapiTest, OH_Sensor_SubscribeSensorTest_001, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_GetAllSensors_001 in");
    Sensor_SubscribeUser user;
    user.callback = nullptr;
    Sensor_SubscribeId subscribeId;
    struct Sensor_SubscribeAttribute attribute;
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_SubscribeSensor(subscribeId, attribute, &user));
    ASSERT_NE(ret, SENSOR_SUCCESS);
}

// HWTEST_F(SensorCapiTest, OH_Sensor_SubscribeSensorTest_002, TestSize.Level1)
// {
//     SEN_HILOGI("OH_Sensor_SubscribeSensor_002 in");
//     Sensor_SubscribeUser user;
//     user.callback = SensorDataCallbackImpl;
//     struct Sensor_SubscribeAttribute attribute;
//     Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_SubscribeSensor(INVALID_VALUE, attribute, &user));
//     ASSERT_NE(ret, SENSOR_SUCCESS);
// }

HWTEST_F(SensorCapiTest, OH_Sensor_SubscribeSensorTest_003, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_SubscribeSensor_003 in");
    struct Sensor_SubscribeAttribute attribute;
    Sensor_SubscribeId subscribeId;
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_SubscribeSensor(subscribeId, attribute, nullptr));
    ASSERT_NE(ret, SENSOR_SUCCESS);
}

HWTEST_F(SensorCapiTest, OH_Sensor_UnsubscribeSensorTest_001, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_UnsubscribeSensorTest_001 in");
    Sensor_SubscribeUser user;
    user.callback = nullptr;
    struct Sensor_SubscribeAttribute attribute;
    Sensor_SubscribeId subscribeId;
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_SubscribeSensor(subscribeId, attribute, &user));
    ASSERT_NE(ret, SENSOR_SUCCESS);
    ret = static_cast<Sensor_Result>(OH_Sensor_UnsubscribeSensor(subscribeId, &user));
    ASSERT_NE(ret, SENSOR_SUCCESS);
}

// HWTEST_F(SensorCapiTest, OH_Sensor_UnsubscribeSensorTest_002, TestSize.Level1)
// {
//     SEN_HILOGI("OH_Sensor_UnsubscribeSensorTest_002 in");
//     Sensor_SubscribeUser user;
//     user.callback = SensorDataCallbackImpl;
//     Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_UnsubscribeSensor(INVALID_VALUE, &user));
//     ASSERT_NE(ret, SENSOR_SUCCESS);
// }

HWTEST_F(SensorCapiTest, OH_Sensor_UnsubscribeSensorTest_003, TestSize.Level1)
{
    SEN_HILOGI("OH_Sensor_UnsubscribeSensor_003 in");
    Sensor_SubscribeId subscribeId;
    Sensor_Result ret = static_cast<Sensor_Result>(OH_Sensor_UnsubscribeSensor(subscribeId, nullptr));
    ASSERT_NE(ret, SENSOR_SUCCESS);
}
}  // namespace Sensors
}  // namespace OHOS
