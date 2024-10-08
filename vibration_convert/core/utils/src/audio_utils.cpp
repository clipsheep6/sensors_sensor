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

#include "audio_utils.h"
#include "sensors_errors.h"

#undef LOG_TAG
#define LOG_TAG "AudioUtils"

namespace OHOS {
namespace Sensors {
namespace {
constexpr int32_t MTOF_ARRAY_SIZE = 128;
// This is a lookup table for converting midi to frequency
constexpr double MTOF_ARRAY[MTOF_ARRAY_SIZE + 1] = {
    0, 8.661957, 9.177024, 9.722718, 10.3, 10.913383, 11.562325, 12.25, 12.978271,
    13.75, 14.567617, 15.433853, 16.351599, 17.323914, 18.354048, 19.445436, 20.601723, 21.826765, 23.124651,
    24.5, 25.956543, 27.5, 29.135235, 30.867706, 32.703197, 34.647827, 36.708096, 38.890873, 41.203445,
    43.65353, 46.249302, 49., 51.913086, 55., 58.27047, 61.735413, 65.406395, 69.295654, 73.416191, 77.781746,
    82.406891, 87.30706, 92.498604, 97.998856, 103.826172, 110., 116.540939, 123.470825, 130.81279, 138.591309,
    146.832382, 155.563492, 164.813782, 174.61412, 184.997208, 195.997711, 207.652344, 220., 233.081879, 246.94165,
    261.62558, 277.182617, 293.664764, 311.126984, 329.627563, 349.228241, 369.994415, 391.995422, 415.304688, 440.,
    466.163757, 493.883301, 523.25116, 554.365234, 587.329529, 622.253967, 659.255127, 698.456482, 739.988831,
    783.990845, 830.609375, 880., 932.327515, 987.766602, 1046.502319, 1108.730469, 1174.659058, 1244.507935,
    1318.510254, 1396.912964, 1479.977661, 1567.981689, 1661.21875, 1760., 1864.655029, 1975.533203, 2093.004639,
    2217.460938, 2349.318115, 2489.015869, 2637.020508, 2793.825928, 2959.955322, 3135.963379, 3322.4375, 3520.,
    3729.31, 3951.066406, 4186.009277, 4434.921875, 4698.63623, 4978.031738, 5274.041016, 5587.651855,
    5919.910645, 6271.926758, 6644.875, 7040., 7458.620117, 7902.132812, 8372.018555, 8869.84375, 9397.272461,
    9956.063477, 10548.082031, 11175.303711, 11839.821289, 12543.853516, 13289.75 };
} // namespace

double AudioUtils::ConvertMtof(int32_t midinote)
{
    if (midinote < 0) {
        midinote = 0;
    }
    if (midinote > MTOF_ARRAY_SIZE) {
        midinote = MTOF_ARRAY_SIZE;
    }
    return MTOF_ARRAY[midinote];
}

std::vector<double> AudioUtils::PadData(const std::vector<double> &data, int32_t hopLength)
{
    CALL_LOG_ENTER;
    std::vector<double> paddingData;
    paddingData.insert(paddingData.end(), hopLength / 2, 0);
    paddingData.insert(paddingData.end(), data.begin(), data.end());
    paddingData.insert(paddingData.end(), hopLength / 2, 0);
    return paddingData;
}
} // namespace Sensors
} // namespace OHOS