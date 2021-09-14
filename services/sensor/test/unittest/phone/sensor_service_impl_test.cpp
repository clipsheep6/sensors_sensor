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
#include "sensor_agent_type.h"
#include "sensor_data_channel.h"
#include "sensor_if.h"
#include "sensor_service_client.h"
#include "sensor_service_impl.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_TEST, "SensorServiceImplTest" };
}  // namespace

class SensorServiceImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    SensorServiceImpl &sensorServiceImpl_ = SensorServiceImpl::GetInstance();
};

void SensorServiceImplTest::SetUpTestCase()
{}

void SensorServiceImplTest::TearDownTestCase()
{}

void SensorServiceImplTest::SetUp()
{}

void SensorServiceImplTest::TearDown()
{}

/*
 * @tc.name: RegisteDataReport_001
 * @tc.desc: register data report.
 * @tc.type: FUNC
 * @tc.require: SR000F5A2Q AR000F8QO2
 * @tc.author: wuzhihui
 */
HWTEST_F(SensorServiceImplTest, RegisteDataReport_001, TestSize.Level1)
{
    HiLog::Info(LABEL, "%{public}s begin", __func__);
    // 获取sensor列表
    sensorServiceImpl_.InitSensorServiceImpl();
    std::vector<Sensor> list = sensorServiceImpl_.GetSensorList();
    for (size_t i=0; i<list.size(); i++) {
        Sensor sensor = list[i];
        printf("get sensoriId[%d], sensorName[%s], vendorNmae[%s]\n\r", sensor.GetSensorId(), 
                sensor.GetName().c_str(), sensor.GetVendor().c_str());
    }
    ASSERT_NE(list.size(), 0UL);
    // SetBatch
    const int32_t sensorId = 0;
    const int32_t sensorInterval = 1000000000;
    const int32_t sensorPollTime = 5;
    int32_t ret = sensorServiceImpl_.SetSensorConfig(sensorId, sensorInterval, sensorPollTime);
    EXPECT_EQ(0, ret);

    // 注册
    ZReportDataCb cb;
    sptr<ReportDataCallback> reportDataCallback_;
    reportDataCallback_ = new (std::nothrow) ReportDataCallback();
    ASSERT_NE(reportDataCallback_, nullptr);
    cb = &ReportDataCallback::ZReportDataCallback;
    ret = sensorServiceImpl_.RegisteDataReport(cb, reportDataCallback_);
    ASSERT_EQ(ret, 0);

    // 激活
    ret = sensorServiceImpl_.EnableSensor(sensorId);
    EXPECT_EQ(0, ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    HiLog::Info(LABEL, "%{public}s begin", "--->Unregister1111");
    // 去注册
    ret = sensorServiceImpl_.Unregister();
    ASSERT_EQ(ret, 0);
    HiLog::Info(LABEL, "%{public}s begin", "--->Unregister222");

    // 去激活
    ret = sensorServiceImpl_.DisableSensor(sensorId);
    ASSERT_EQ(ret, 0);
    HiLog::Info(LABEL, "%{public}s end", __func__);
}
}  // namespace Sensors
}  // namespace OHOS
