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

#include "sensor_file_descriptor_listener.h"
#include "sensor_agent_type.h"
#include "sensor_basic_data_channel.h"
#include "sensors_errors.h"
#include "sys/socket.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
using namespace OHOS::AppExecFwk;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorFileDescriptorListener" };
constexpr int32_t RECEIVE_DATA_SIZE = 100;
constexpr int32_t MAX_RETRY_NUMBER = 32;
}  // namespace

SensorFileDescriptorListener::SensorFileDescriptorListener()
{
    channel_ = nullptr;
}

SensorFileDescriptorListener::~SensorFileDescriptorListener()
{
    CALL_LOG_ENTER;
}

int32_t SensorFileDescriptorListener::ReceiveData(int32_t fileDescriptor, void *vaddr, size_t size)
{
    if (fileDescriptor < 0 || vaddr == nullptr) {
        SEN_HILOGE("Invalid fileDescriptor or vaddr is null, fileDescriptor:%{public}d", fileDescriptor);
        return 0;
    }
    ssize_t length = 0;
    int32_t retryNumber = 0;
    while (true) {
        ssize_t curLength = recv(fileDescriptor, vaddr, size, MSG_DONTWAIT | MSG_NOSIGNAL);
        if (curLength > 0) {
            length += curLength;
        } else if (curLength < 0) {
            if (retryNumber < MAX_RETRY_NUMBER && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK)) {
                SEN_HILOGE("Need retry, errno is EAGAIN|EINTR|EWOULDBLOCK, curLength:%{public}zu, errno:%{public}d",
                    curLength, errno);
                retryNumber++;
                continue;
            }
            SEN_HILOGE("Recv return error, curLength:%{public}zu, errno:%{public}d", curLength, errno);
            break;
        } else {
            SEN_HILOGE("curLength:0, errno:%{public}d", errno);
            break;
        }
        if (curLength < size) {
            break;
        }
    }
    return length;
}

void SensorFileDescriptorListener::OnReadable(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        SEN_HILOGE("Invalid fileDescriptor:%{public}d", fileDescriptor);
        return;
    }
    FileDescriptorListener::OnReadable(fileDescriptor);
    TransferSensorEvents receiveDataBuff[RECEIVE_DATA_SIZE] = {};
    int32_t len = ReceiveData(fileDescriptor, receiveDataBuff, sizeof(TransferSensorEvents) * RECEIVE_DATA_SIZE);
    int32_t eventSize = static_cast<int32_t>(sizeof(TransferSensorEvents));
    int32_t num = len / eventSize;
    for (int i = 0; i < num; i++) {
        SensorEvent event = {
            .sensorTypeId = receiveDataBuff[i].sensorTypeId,
            .version = receiveDataBuff[i].version,
            .timestamp = receiveDataBuff[i].timestamp,
            .option = receiveDataBuff[i].option,
            .mode = receiveDataBuff[i].mode,
            .data = receiveDataBuff[i].data,
            .dataLen = receiveDataBuff[i].dataLen
        };
        channel_->dataCB_(&event, 1, channel_->privateData_);
    }
}

void SensorFileDescriptorListener::OnWritable(int32_t fileDescriptor) {}

void SensorFileDescriptorListener::SetChannel(SensorDataChannel* channel)
{
    channel_ = channel;
}

void SensorFileDescriptorListener::OnShutdown(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        SEN_HILOGE("param is error:%{public}d", fileDescriptor);
    }
    FileDescriptorListener::OnShutdown(fileDescriptor);
}

void SensorFileDescriptorListener::OnException(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        SEN_HILOGE("param is error:%{public}d", fileDescriptor);
        return;
    }
    FileDescriptorListener::OnException(fileDescriptor);
}
}  // namespace Sensors
}  // namespace OHOS