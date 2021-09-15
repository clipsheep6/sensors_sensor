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

#include "sensor.h"

#include "sensors_errors.h"
#include "sensors_log_domain.h"
namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_UTILS, "Sensor" };
}

Sensor::Sensor()
    : sensorId_(0),
      version_(0),
      maxRange_(0.0),
      resolution_(0.0),
      flags_(0),
      fifoMaxEventCount_(0),
      minSamplePeriodNs_(0),
      maxSamplePeriodNs_(0)
{}

uint32_t Sensor::GetSensorId() const
{
    return sensorId_;
}

void Sensor::SetSensorId(uint32_t sensorId)
{
    sensorId_ = sensorId;
}

std::string Sensor::GetName() const
{
    return name_;
}

void Sensor::SetName(const std::string &name)
{
    name_ = name;
}

std::string Sensor::GetVendor() const
{
    return vendor_;
}

void Sensor::SetVendor(const std::string &vendor)
{
    vendor_ = vendor;
}

uint32_t Sensor::GetVersion() const
{
    return version_;
}

void Sensor::SetVersion(uint32_t version)
{
    version_ = version;
}

float Sensor::GetMaxRange() const
{
    return maxRange_;
}

void Sensor::SetMaxRange(float maxRange)
{
    maxRange_ = maxRange;
}

float Sensor::GetResolution() const
{
    return resolution_;
}

void Sensor::SetResolution(float resolution)
{
    resolution_ = resolution;
}

uint32_t Sensor::GetFlags() const
{
    return flags_;
}

void Sensor::SetFlags(uint32_t flags)
{
    flags_ = flags;
}

int32_t Sensor::GetFifoMaxEventCount() const
{
    return fifoMaxEventCount_;
}

void Sensor::SetFifoMaxEventCount(int32_t fifoMaxEventCount)
{
    fifoMaxEventCount_ = fifoMaxEventCount;
}

int64_t Sensor::GetMinSamplePeriodNs() const
{
    return minSamplePeriodNs_;
}

void Sensor::SetMinSamplePeriodNs(int64_t minSamplePeriodNs)
{
    minSamplePeriodNs_ = minSamplePeriodNs;
}

int64_t Sensor::GetMaxSamplePeriodNs() const
{
    return maxSamplePeriodNs_;
}

void Sensor::SetMaxSamplePeriodNs(int64_t maxSamplePeriodNs)
{
    maxSamplePeriodNs_ = maxSamplePeriodNs;
}

std::vector<uint32_t> Sensor::GetReserved() const
{
    return reserved_;
}

void Sensor::SetReserved(const std::vector<uint32_t> &reserved)
{
    auto reservedCount = reserved.size();
    for (size_t i = 0; i < reservedCount; i++) {
        reserved_[i] = reserved[i];
    }
}

bool Sensor::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(sensorId_)) {
        HiLog::Error(LABEL, "%{public}s failed, write sensorId failed", __func__);
        return false;
    }
    if (!parcel.WriteString(name_)) {
        HiLog::Error(LABEL, "%{public}s failed, write name_ failed", __func__);
        return false;
    }
    if (!parcel.WriteString(vendor_)) {
        HiLog::Error(LABEL, "%{public}s failed, write vendor_ failed", __func__);
        return false;
    }
    if (!parcel.WriteUint32(version_)) {
        HiLog::Error(LABEL, "%{public}s failed, write version_ failed", __func__);
        return false;
    }
    if (!parcel.WriteFloat(maxRange_)) {
        HiLog::Error(LABEL, "%{public}s failed, write maxRange_ failed", __func__);
        return false;
    }
    if (!parcel.WriteFloat(resolution_)) {
        HiLog::Error(LABEL, "%{public}s failed, write resolution_ failed", __func__);
        return false;
    }
    if (!parcel.WriteUint32(flags_)) {
        HiLog::Error(LABEL, "%{public}s failed, write flags_ failed", __func__);
        return false;
    }
    if (!parcel.WriteInt32(fifoMaxEventCount_)) {
        HiLog::Error(LABEL, "%{public}s failed, write fifoMaxEventCount_ failed", __func__);
        return false;
    }
    if (!parcel.WriteInt64(minSamplePeriodNs_)) {
        HiLog::Error(LABEL, "%{public}s failed, write minSamplePeriodNs_ failed", __func__);
        return false;
    }
    if (!parcel.WriteInt64(maxSamplePeriodNs_)) {
        HiLog::Error(LABEL, "%{public}s failed, write maxSamplePeriodNs_ failed", __func__);
        return false;
    }
    if (!parcel.WriteUInt32Vector(reserved_)) {
        HiLog::Error(LABEL, "%{public}s failed, write reserved_ failed", __func__);
        return false;
    }

    return true;
}

std::unique_ptr<Sensor> Sensor::Unmarshalling(Parcel &parcel)
{
    auto sensor = std::make_unique<Sensor>();
    if (sensor == nullptr) {
        HiLog::Error(LABEL, "%{public}s sensor cannot be null", __func__);
        return nullptr;
    }

    if (!sensor->ReadFromParcel(parcel)) {
        HiLog::Error(LABEL, "%{public}s ReadFromParcel failed", __func__);
        return nullptr;
    }
    return sensor;
}

bool Sensor::ReadFromParcel(Parcel &parcel)
{
    sensorId_ = parcel.ReadUint32();
    name_ = parcel.ReadString();
    vendor_ = parcel.ReadString();
    version_ = parcel.ReadUint32();
    maxRange_ = parcel.ReadFloat();
    resolution_ = parcel.ReadFloat();
    flags_ = parcel.ReadUint32();
    fifoMaxEventCount_ = parcel.ReadInt32();
    minSamplePeriodNs_ = parcel.ReadInt64();
    maxSamplePeriodNs_ = parcel.ReadInt64();
    return parcel.ReadUInt32Vector(&reserved_);
}
}  // namespace Sensors
}  // namespace OHOS
