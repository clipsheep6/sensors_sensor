/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "setmode_fuzzer.h"

#include "securec.h"

#include "sensor_agent.h"
#include "sensor_agent_type.h"

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool SetModeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < (sizeof(int32_t) + sizeof(SensorUser))) {
        return false;
    }
    int32_t option;
    size_t startPos = 0;
    startPos = GetObject<int32_t>(option, data + startPos, size - startPos);
    SensorUser user;
    GetObject<SensorUser>(user, data + startPos, size - startPos);
    if (SetMode(option, &user, option) != 0) {
        return false;
    }
    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    SetModeFuzzTest(data, size);
    return 0;
}

