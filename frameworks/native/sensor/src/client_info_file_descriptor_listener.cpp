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

#include "client_info_file_descriptor_listener.h"

#include <cinttypes>

#include "sensors_errors.h"
#include "stream_socket.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::AppExecFwk;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "ClientInfoFileDescriptorListener" };
}

void ClientInfoFileDescriptorListener::SetChannel(SensorDataChannel *channel)
{
    channel_ = channel;
}

void ClientInfoFileDescriptorListener::OnReadable(int32_t fd)
{
    if (fd < 0) {
        SEN_HILOGE("Invalid fd");
        return;
    }
    char szBuf[PROTO_MAX_PACKET_BUF_SIZE] = {};
    for (int32_t i = 0; i < PROTO_MAX_RECV_LIMIT; i++) {
        ssize_t size = recv(fd, szBuf, PROTO_MAX_PACKET_BUF_SIZE, MSG_DONTWAIT | MSG_NOSIGNAL);
        if (size > 0) {
            channel_->receiveMessage_(szBuf, size);
        } else if (size < 0) {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
                SEN_HILOGW("Continue for errno EAGAIN|EINTR|EWOULDBLOCK, size:%{public}zu, errno:%{public}d",
                           size, errno);
                continue;
            }
            SEN_HILOGE("Recv return %{public}zu, errno:%{public}d", size, errno);
            break;
        } else {
            SEN_HILOGD("The service side disconnect with the client. size:0, count:%{public}d, errno:%{public}d",
                       i, errno);
            break;
        }       
        if (size < PROTO_MAX_PACKET_BUF_SIZE) {
            break;
        }
    }
}

void ClientInfoFileDescriptorListener::OnShutdown(int32_t fd)
{
    if (fd < 0) {
        SEN_HILOGE("Invalid fd");
    }
    channel_->disconnect_();
}

void ClientInfoFileDescriptorListener::OnException(int32_t fd)
{
    if (fd < 0) {
        SEN_HILOGE("Invalid fd");
    }
    channel_->disconnect_();
}
}  // namespace Sensors
}  // namespace OHOS
