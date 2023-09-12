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

/**
 * @addtogroup 传感器
 * @{
 *
 * @brief 为您提供标准的开放api，以使用传感器的常用功能。
 *
 * 例如，您可以调用这些api来获取传感器属性信息、订阅或取消订阅传感器数据等。
 * @since 11
 */

/**
 * @file sensor_capi.h
 *
 * @brief 声明操作传感器的常用api，用于获取传感器信息、订阅或取消订阅传感器数据等。
 * @syscap SystemCapability.Sensors.Sensor
 * @since 11
 */

#ifndef SENSOR_CAPI_H
#define SENSOR_CAPI_H

#include <stdint.h>

#include "sensor_capi_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取设备上的所有传感器信息。
 *
 * @param sensorInfo 设备上的所有传感器信息，
 * 详情请参见{@Link Sensor_SensorInfo}。
 * @param count 设备上的传感器数量。
 * @return 获取传感器信息成功返回SENSOR_SUCCESS，否则返回对应的错误码,
 * 详情请参见{@Link Sensor_Result}。
 *
 * @since 11
 */
Sensor_Result OH_Sensor_GetAllSensors(Sensor_SensorInfo **sensorInfo, int32_t *count);

/**
 * @brief 订阅传感器数据，系统将按照用户指定的上报频率向用户报告传感器数据。
 *
 * @param sensorTypeId 传感器类型, 详情请参见{@link Sensor_SensorTypeId}。
 * @param attribute 订阅属性，用于指定传感器的上报频率，详情请参见{@Link Sensor_SubscribeAttribute}。
 * @param user 订阅者信息，指定传感器数据回调函数，详情请参见{@Link Sensor_SensorUser}。
 * @return 订阅传感器成功返回SENSOR_SUCCESS，否则返回对应的错误码,
 * 详情请参见{@Link Sensor_Result}。
 *
 * @since 11
 */
Sensor_Result OH_Sensor_SubscribeSensor(Sensor_SensorTypeId sensorTypeId, Sensor_SubscribeAttribute attribute,
    const Sensor_SensorUser *user);

/**
 * @brief 取消订阅传感器数据。
 *
 * @param sensorTypeId 传感器类型, 详情请参见{@link Sensor_SensorTypeId}。
 * @param user 订阅者信息，指定传感器数据回调函数，
 * 详情请参见{@Link Sensor_SensorUser}。
 * @return 取消订阅传感器成功返回SENSOR_SUCCESS，否则返回对应的错误码,
 * 详情请参见{@Link Sensor_Result}。
 *
 * @since 11
 */
Sensor_Result OH_Sensor_UnsubscribeSensor(Sensor_SensorTypeId sensorTypeId, const Sensor_SensorUser *user);

#ifdef __cplusplus
}
#endif
#endif // SENSOR_CAPI_H