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
#include <gtest/gtest.h>

#include "report_data_callback.h"
#include "sensor_errors.h"

#undef LOG_TAG
#define LOG_TAG "ReportDataCallbackTest"

namespace OHOS {
namespace Sensors {
using namespace testing::ext;
namespace {
    constexpr uint8_t BLOCK_EVENT_BUF_LEN = 16;
    SensorData* g_sensorData = new (std::nothrow) SensorData[BLOCK_EVENT_BUF_LEN];
} // namespace

class SensorBasicDataChannelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SensorBasicDataChannelTest::SetUpTestCase() {}

void SensorBasicDataChannelTest::TearDownTestCase()
{
    if (g_sensorData != nullptr) {
        delete[] g_sensorData;
        g_sensorData = nullptr;
    }
}

void SensorBasicDataChannelTest::SetUp() {}

void SensorBasicDataChannelTest::TearDown() {}

HWTEST_F(SensorBasicDataChannelTest, ReportDataCallbackTest_001, TestSize.Level1)
{
    SEN_HILOGI("ReportDataCallbackTest_001 in");

    ReportDataCallback reportDataCallback = ReportDataCallback();
    ReportDataCallback *cb = nullptr;
    int32_t ret = reportDataCallback.ReportEventCallback(g_sensorData, cb);
    ASSERT_EQ(ret, ERROR);

    sptr<ReportDataCallback> callback = new (std::nothrow) ReportDataCallback();
    if (!callback->eventsBuf_.blockList.empty()) {
        callback->eventsBuf_.blockList.clear();
    }
    callback->eventsBuf_.blockList.clear();
    ret = reportDataCallback.ReportEventCallback(g_sensorData, callback);
    ASSERT_EQ(ret, ERROR);
}

HWTEST_F(SensorBasicDataChannelTest, ReportDataCallbackTest_002, TestSize.Level1)
{
    SEN_HILOGI("ReportDataCallbackTest_002 in");

    ReportDataCallback reportDataCallback = ReportDataCallback();
    sptr<ReportDataCallback> callback = new (std::nothrow) ReportDataCallback();

    callback->eventsBuf_.writeFullBlockNum = -1;
    int32_t ret = reportDataCallback.ReportEventCallback(g_sensorData, callback);
    ASSERT_EQ(ret, ERROR);
}

HWTEST_F(SensorBasicDataChannelTest, ReportDataCallbackTest_003, TestSize.Level1)
{
    SEN_HILOGI("ReportDataCallbackTest_003 in");
    ReportDataCallback reportDataCallback = ReportDataCallback();
    sptr<ReportDataCallback> callback = new (std::nothrow) ReportDataCallback();
    callback->eventsBuf_.writeFullBlockNum  = BLOCK_EVENT_BUF_LEN;
    int32_t ret = reportDataCallback.ReportEventCallback(g_sensorData, callback);
    ASSERT_EQ(ret, ERR_OK);
}

HWTEST_F(SensorBasicDataChannelTest, ReportDataCallbackTest_004, TestSize.Level1)
{
    SEN_HILOGI("ReportDataCallbackTest_004 in");
    ReportDataCallback reportDataCallback = ReportDataCallback();
    sptr<ReportDataCallback> callback = new (std::nothrow) ReportDataCallback();
    callback->eventsBuf_.writeFullBlockNum  = BLOCK_EVENT_BUF_LEN + 1;
    int32_t ret = reportDataCallback.ReportEventCallback(g_sensorData, callback);
    ASSERT_EQ(ret, ERR_OK);
}

HWTEST_F(SensorBasicDataChannelTest, ReportDataCallbackTest_005, TestSize.Level1)
{
    SEN_HILOGI("ReportDataCallbackTest_005 in");
    ReportDataCallback reportDataCallback = ReportDataCallback();
    sptr<ReportDataCallback> callback = new (std::nothrow) ReportDataCallback();
    callback->eventsBuf_.writeFullBlockNum  = BLOCK_EVENT_BUF_LEN - 1;
    int32_t ret = reportDataCallback.ReportEventCallback(g_sensorData, callback);
    ASSERT_EQ(ret, ERR_OK);
}
} // namespace Sensors
} // namespace OHOS