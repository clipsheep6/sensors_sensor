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

/**
 * @addtogroup PanSensor
 * @{
 *
 * @brief Provides standard open APIs for you to use common capabilities of sensors.
 *
 * For example, you can call these APIs to obtain sensor information,
 * subscribe to or unsubscribe from sensor data, enable or disable a sensor,
 * and set the sensor data reporting mode.
 *
 * @since 5
 */

/**
 * @file sensor_agent_type.h
 *
 * @brief Defines the basic data used by the sensor agent to manage sensors.
 *
 * @since 5
 */

#ifndef SENSOR_AGENT_TYPE_H
#define SENSOR_AGENT_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/** Maximum length of the sensor name */
#ifndef SENSOR_NAME_MAX_LEN2
#define SENSOR_NAME_MAX_LEN2 48
#endif /* SENSOR_NAME_MAX_LEN */
/** Size of sensor data */
#ifndef SENSOR_USER_DATA_SIZE
#define SENSOR_USER_DATA_SIZE 104
#endif /* SENSOR_USER_DATA_SIZE */
/** Maximum length of the sensor version */
#ifndef VERSION_MAX_LEN
#define VERSION_MAX_LEN 16
#endif /* SENSOR_USER_DATA_SIZE */

/**
 * @brief Enumerates sensor types.
 *
 * @since 5
 */
typedef enum SensorTypeId {
    SENSOR_TYPE_ID_NONE = 0,                   /**< None */
    SENSOR_TYPE_ID_ACCELEROMETER = 1,          /**< Acceleration sensor */
    SENSOR_TYPE_ID_GYROSCOPE = 2,              /**< Gyroscope sensor */
    SENSOR_TYPE_ID_PHOTOPLETHYSMOGRAPH = 3,    /**< Photoplethysmography sensor */
    SENSOR_TYPE_ID_ELECTROCARDIOGRAPH = 4,     /**< Electrocardiogram (ECG) sensor */
    SENSOR_TYPE_ID_AMBIENT_LIGHT = 5,          /**< Ambient light sensor */
    SENSOR_TYPE_ID_MAGNETIC_FIELD = 6,         /**< Magnetic field sensor */
    SENSOR_TYPE_ID_CAPACITIVE = 7,             /**< Capacitive sensor */
    SENSOR_TYPE_ID_BAROMETER = 8,              /**< Barometric pressure sensor */
    SENSOR_TYPE_ID_TEMPERATURE = 9,            /**< Temperature sensor */
    SENSOR_TYPE_ID_HALL = 10,                  /**< Hall effect sensor */
    SENSOR_TYPE_ID_GESTURE = 11,               /**< Gesture sensor */
    SENSOR_TYPE_ID_PROXIMITY = 12,             /**< Proximity sensor */
    SENSOR_TYPE_ID_HUMIDITY = 13,              /**< Humidity sensor */
    SENSOR_TYPE_ID_PHYSICAL_MAX = 0xFF,        /**< Maximum type ID of a physical sensor */
    SENSOR_TYPE_ID_ORIENTATION = 256,          /**< Orientation sensor */
    SENSOR_TYPE_ID_GRAVITY = 257,              /**< Gravity sensor */
    SENSOR_TYPE_ID_LINEAR_ACCELERATION = 258,  /**< Linear acceleration sensor */
    SENSOR_TYPE_ID_ROTATION_VECTOR = 259,      /**< Rotation vector sensor */
    SENSOR_TYPE_ID_AMBIENT_TEMPERATURE = 260,  /**< Ambient temperature sensor */
    SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED = 261,  /**< Uncalibrated magnetic field sensor */
    SENSOR_TYPE_ID_GAME_ROTATION_VECTOR = 262,    /**< Game rotation vector sensor */
    SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED = 263,  /**< Uncalibrated gyroscope sensor */
    SENSOR_TYPE_ID_SIGNIFICANT_MOTION = 264,    /**< Significant motion sensor */
    SENSOR_TYPE_ID_PEDOMETER_DETECTION = 265,   /**< Pedometer detection sensor */
    SENSOR_TYPE_ID_PEDOMETER = 266,             /**< Pedometer sensor */
    SENSOR_TYPE_ID_GEOMAGNETIC_ROTATION_VECTOR = 277,  /**< Geomagnetic rotation vector sensor */
    SENSOR_TYPE_ID_HEART_RATE = 278,            /**< Heart rate sensor */
    SENSOR_TYPE_ID_DEVICE_ORIENTATION = 279,    /**< Device orientation sensor */
    SENSOR_TYPE_ID_WEAR_DETECTION = 280,        /**< Wear detection sensor */
    SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED = 281,   /**< Uncalibrated acceleration sensor */
    SENSOR_TYPE_ID_MAX = 30,      /**< Maximum number of sensor type IDs*/
} SensorTypeId;

/**
 * @brief Defines sensor information.
 *
 * @since 5
 */
typedef struct SensorInfo {
    char sensorName[SENSOR_NAME_MAX_LEN2];   /**< Sensor name */
    char vendorName[SENSOR_NAME_MAX_LEN2];   /**< Sensor vendor */
    char firmwareVersion[VERSION_MAX_LEN];  /**< Sensor firmware version */
    char hardwareVersion[VERSION_MAX_LEN];  /**< Sensor hardware version */
    int32_t sensorTypeId;  /**< Sensor type ID */
    int32_t sensorId;      /**< Sensor ID */
    float maxRange;        /**< Maximum measurement range of the sensor */
    float precision;       /**< Sensor accuracy */
    float power;           /**< Sensor power */
} SensorInfo;

/**
 * @brief Defines the data reported by the sensor.
 *
 * @since 5
 */
typedef struct SensorEvent {
    int32_t sensorTypeId;  /**< Sensor type ID */
    int32_t version;       /**< Sensor algorithm version */
    int64_t timestamp;     /**< Time when sensor data was reported */
    uint32_t option;       /**< Sensor data options, including the measurement range and accuracy */
    int32_t mode;          /**< Sensor data reporting mode (described in {@link SensorMode}) */
    uint8_t *data;         /**< Sensor data */
    uint32_t dataLen;      /**< Sensor data length */
} SensorEvent;

struct Test {
char ip[30];
char name[30];
int32_t *data;
};

/**
 * @brief Defines the callback for data reporting by the sensor agent.
 *
 * @since 5
 */
typedef void (*RecordSensorCallback)(SensorEvent *event);

/**
 * @brief Defines a reserved field for the sensor data subscriber.
 *
 * @since 5
 */
typedef struct UserData {
    char userData[SENSOR_USER_DATA_SIZE];  /**< Reserved for the sensor data subscriber */
} UserData;

/**
 * @brief Defines information about the sensor data subscriber.
 *
 * @since 5
 */
typedef struct SensorUser {
    char name[SENSOR_NAME_MAX_LEN2];  /**< Name of the sensor data subscriber */
    RecordSensorCallback callback;   /**< Callback for reporting sensor data */
    UserData *userData;              /**< Reserved field for the sensor data subscriber */
} SensorUser;

/**
 * @brief Enumerates data reporting modes of sensors.
 *
 * @since 5
 */
typedef enum SensorMode {
    SENSOR_DEFAULT_MODE = 0,   /**< Default data reporting mode */
    SENSOR_REALTIME_MODE = 1,  /**< Real-time data reporting mode to report a group of data each time */
    SENSOR_ON_CHANGE = 2,   /**< Real-time data reporting mode to report data upon status changes */
    SENSOR_ONE_SHOT = 3,    /**< Real-time data reporting mode to report data only once */
    SENSOR_FIFO_MODE = 4,   /**< FIFO-based data reporting mode to report data based on the <b>BatchCnt</b> setting */
    SENSOR_MODE_MAX2,        /**< Maximum sensor data reporting mode */
} SensorMode;

/**
 * @brief 加速度传感器的数据结构
 */
typedef struct AccelData {
    /**< 加速度X轴 */
    int32_t axisX;
    /**< 加速度Y轴 */
    int32_t axisY;
    /**< 加速度Z轴 */
    int32_t axisZ;
    /**< 校准精度 */
    int32_t accuracy;
} AccelData;

/**
 * @brief 线性加速度传感器的数据结构
 */
typedef struct LinearAccelData {
    /**< 线性加速度X轴 */
    int32_t axisX;
    /**< 线性加速度Y轴 */
    int32_t axisY;
    /**< 线性加速度Z轴 */
    int32_t axisZ;
    /**< 校准精度 */
    int32_t accuracy;
} LineraAccelData;

/**
 * @brief 陀螺仪传感器的数据结构
 */
typedef struct GyroscopeData {
    /**< 加速度X轴 */
    int32_t axisX;
    /**< 加速度Y轴 */
    int32_t axisY;
    /**< 加速度Z轴 */
    int32_t axisZ;
    /**< 校准精度 */
    int32_t accuracy;
} GyroscopeData;

/**
 * @brief 重力传感器的数据结构
 */
typedef struct GravityData {
    /**< 重力加速度X轴 */
    int32_t axisX;
    /**< 重力加速度Y轴 */
    int32_t axisY;
    /**< 重力加速度Z轴 */
    int32_t axisZ;
    /**< 校准精度 */
    int32_t accuracy;
} GravityData;

/**
 * @brief 未校准加速度传感器的数据结构
 */
typedef struct AccelUncalibratedData {
    /**< 未校准加速度X轴 */
    int32_t axisX;
    /**< 未校准加速度Y轴 */
    int32_t axisY;
    /**< 未校准加速度Z轴 */
    int32_t axisZ;
    /**< 未校准加速度X轴偏量 */
    int32_t axisBiasX;
    /**< 未校准加速度Y轴偏量 */
    int32_t axisBiasY;
    /**< 未校准加速度Z轴偏量 */
    int32_t axisBiasZ;
} AccelUncalibratedData;

/**
 * @brief 未校准陀螺仪传感器的数据结构
 */
typedef struct GyroUncalibratedData {
    /**< 未校准加速度X轴 */
    int32_t axisX;
    /**< 未校准加速度Y轴 */
    int32_t axisY;
    /**< 未校准加速度Z轴 */
    int32_t axisZ;
    /**< 未校准加速度X轴偏量 */
    int32_t axisBiasX;
    /**< 未校准加速度Y轴偏量 */
    int32_t axisBiasY;
    /**< 未校准加速度Z轴偏量 */
    int32_t axisBiasZ;
} GyroUncalibratedData;

/**
 * @brief 大幅度动作传感器的数据结构
 */
typedef struct SignificantMotionData {
    /**< 标量 */
    int32_t scalar;
} SignificantMotionData;

/**
 * @brief 计步器检测传感器的数据结构
 */
typedef struct PedometerDetectData {
    /**< 标量 */
    int32_t scalar;
} PedometerDetectData;

/**
 * @brief 计步器传感器的数据结构
 */
typedef struct PedometerData {
    /**< 标量 */
    int32_t scalar;
} PedometerData;

/**
 * @brief  环境温度传感器的数据结构
 */
typedef struct AmbientTemperatureData {
    /**< 标量 */
    int32_t scalar;
} AmbientTemperatureData;

/**
 * @brief 湿度传感器的数据结构
 */
typedef struct HumidityData {
    /**< 标量 */
    int32_t scalar;
} HumidityData;

/**
 * @brief 磁场传感器的数据结构
 */
typedef struct MagneticFieldData {
    /**< 磁力计X轴 */
    int32_t axisX;
    /**< 磁力计Y轴 */
    int32_t axisY;
    /**< 磁力计Z轴 */
    int32_t axisZ;
    /**< 磁力计数据精度 */
    int32_t accuracy;
} MagneticFieldData;

/**
 * @brief 未校准磁场传感器的数据结构
 */
typedef struct MagneticFieldUncalibratedData {
    /**< 磁力计X轴 */
    int32_t axisX;
    /**< 磁力计Y轴 */
    int32_t axisY;
    /**< 磁力计Z轴 */
    int32_t axisZ;
    /**< 磁力计X轴偏量 */
    int32_t axisBiasX;
    /**< 磁力计Y轴偏量 */
    int32_t axisBiasY;
    /**< 磁力计Z轴偏量 */
    int32_t axisBiasZ;
} MagneticFieldUncalibratedData;

/**
 * @brief 气压计传感器的数据结构
 */
typedef struct BarometerData {
    /**< 气压值 */
    int32_t pressure;
} BarometerData;

/**
 * @brief 设备方向传感器的数据结构
 */
typedef struct DeviceOrientationData {
    /**< 标量 */
    int32_t scalar;
} DeviceOrientationData;

/**
 * @brief 方向传感器的数据结构
 */
typedef struct OrientationData {
    /**< 方位角（磁北方向与 Y 轴之间的夹角） */
    int32_t axisX;
    /**< 俯仰（绕 X 轴旋转，Z 轴向 Y 轴移动时为正值） */
    int32_t axisY;
    /**< 滚动（绕 Y 轴旋转，当 X 轴向 Z 轴移动时为正值）*/
    int32_t axisZ;
    /**< 校准精度 */
    int32_t accuracy;
} OrientationData;

/**
 * @brief 旋转矢量传感器的数据结构
 */
typedef struct RotationVectorData {
    /**< x分量 */
    int32_t axisX;
    /**< y分量 */
    int32_t axisY;
    /**< z分量 */
    int32_t axisZ;
    /**< w分量 */
    int32_t axisW;
} RotationVectorData;

/**
 * @brief 游戏旋转矢量传感器的数据结构
 */
typedef struct GameRotationVectorData {
    /**< x分量 */
    int32_t axisX;
    /**< y分量 */
    int32_t axisY;
    /**< z分量 */
    int32_t axisZ;
    /**< w分量 */
    int32_t axisW;
} GameRotationVectorData;

/**
 * @brief 地磁旋转矢量传感器的数据结构
 */
typedef struct GeomagneticRotaVectorData {
    /**< x分量 */
    int32_t axisX;
    /**< y分量 */
    int32_t axisY;
    /**< z分量 */
    int32_t axisZ;
    /**< w分量 */
    int32_t axisW;
} GeomagneticRotaVectorData;

/**
 * @brief 接近光传感器的数据结构
 */
typedef struct ProximityData {
    /**< 接近程度 */
    int32_t scalar;
} ProximityData;

/**
 * @brief 环境光传感器的数据结构
 */
typedef struct AmbientLightData {
    /**< 环境强弱 */
    int32_t scalar;
} AmbientLightData;

/**
 * @brief 元数据传感器的数据结构
 */
typedef struct MetaData {
    /**< 标量 */
    int32_t scalar;
} MetaData;

/**
 * @brief 心率传感器的数据结构
 */
typedef struct HeartRateData {
    /**< 心率值，单位bpm */
    int32_t heartRateBpm;
    /**< 心率状态 */
    int32_t temperatureStatus;
} HeartRateData;

/**
 * @brief 佩戴检测传感器的数据结构
 */
typedef struct WearDetectionData {
    /**< 标量 */
    int32_t scalar;
} WearDetectionData;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* SENSOR_AGENT_TYPE_H */
/**< @} */