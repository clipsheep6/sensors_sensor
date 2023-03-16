/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'
import osaccount from '@ohos.account.osAccount'
import sensor from '@ohos.sensor'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

var permissionsFlag = {
    PERMISSION_USER_SET:1,
    PERMISSION_USER_FIXED:2,
    PERMISSION_SYSTEM_FIXED:4
}

const ERR_NOT_HAVE_PERMISSION = 201
const PARAMETER_ERROR_CODE = 401
const SERVICE_EXCEPTION_CODE = 14500101
const PARAMETER_ERROR_MSG = 'The parameter invalid.'
const SERVICE_EXCEPTION_MSG = 'Service exception.'
var tokenID = undefined
var permissionsNameUser = 'ohos.permission.ACTIVITY_MOTION'

describe("Pedometer_DetectionJsTest", function () {
    function callback(data) {
        console.info('callback' + JSON.stringify(data));
        expect(typeof(data.scalar)).assertEqual('number');
    }

    function callback2(data) {
        console.info('callback2' + JSON.stringify(data));
        expect(typeof(data.scalar)).assertEqual('number');
    }
 
    beforeAll(async function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll caled')
         try {
            var accountManager = osaccount.getAccountManager();
            var userId = await accountManager.getOsAccountLocalIdFromProcess();
            var appInfo = await bundle.getApplicationInfo('com.example.myapplication', 0, userId);
            tokenID = appInfo.accessTokenId;
            console.log('AccessTokenId accessTokenId:' + appInfo.accessTokenId + ', name' + appInfo.name
                + ' ,bundleName' + appInfo.bundleName)
         } catch(err) {
            console.error('exception in, msg:' + JSON.stringify(err))
         }
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('afterAll caled')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('beforeEach caled')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach caled')
        var atManager = abilityAccessCtrl.createAtManager();
        atManager.revokeUserGrantedPermission(tokenID, permissionsNameUser,
            permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .catch((err) => {
            console.info('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
        })
    })

    /*
     * @tc.name:BarometerJsTest_001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("Pedometer_DetectionJsTest_001", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_001---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback);
                            setTimeout(()=>{
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                done();
                            }, 500);
                        } catch (err) {
                            console.info('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Program run failed');
                        expect(false).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_002
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_002", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_002---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback, {'interval':100000000});
                            setTimeout(()=>{
                                console.info('----------------Pedometer_DetectionJsTest_002 off in-------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                console.info('----------------Pedometer_DetectionJsTest_002 off end------------------');
                                done();
                            }, 500);
                        } catch (err) {
                            console.info('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_003
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_003", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_003---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    function onSensorCallback(data) {
                        console.info('BarometerJsTest_003  callback in');
                        expect(true).assertTrue();
                        done();
                    }
                    if(data) {
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, onSensorCallback, {'interval': 100000000}, 5);
                            setTimeout(()=>{
                                console.info('----------------------Pedometer_DetectionJsTest_003 off in---------------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                console.info('----------------------Pedometer_DetectionJsTest_003 off end---------------------------');
                                done();
                            }, 500);
                        } catch (err) {
                            console.info('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_004
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_004", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_004---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback, {'interval': -100000000});
                            console.info('--------------------Pedometer_DetectionJsTest_004 off in----------------------');
                            sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                            console.info('--------------------Pedometer_DetectionJsTest_004 off end---------------------');
                            done();
                        } catch (err) {
                            console.info('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(error.code).assertEqual(SERVICE_EXCEPTION_CODE);
                            expect(error.message).assertEqual(SERVICE_EXCEPTION_MSG);
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_005
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_005", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_005---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.once(sensor.SensorId.PEDOMETER_DETECTION, callback);
                            setTimeout(()=>{
                                expect(true).assertTrue();
                                done();
                            }, 500);
                        } catch (err) {
                            console.error('Once fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_006
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_006", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_006---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try{
                            sensor.once(sensor.SensorId.PEDOMETER_DETECTION, callback, 5);
                            setTimeout(()=>{
                                expect(true).assertTrue();
                                done();
                            }, 500);
                        } catch (err) {
                            console.error('Once fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_007
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_007", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_007---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try{
                            sensor.once(sensor.SensorId.PEDOMETER_DETECTION, 5);
                        } catch (error) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(error.code).assertEqual(PARAMETER_ERROR_CODE);
                            expect(error.message).assertEqual(PARAMETER_ERROR_MSG);
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_008
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_008", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_008---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.off(-1, callback);
                        } catch (err) {
                            expect(err.code).assertEqual(PARAMETER_ERROR_CODE)
                            expect(err.message).assertEqual(PARAMETER_ERROR_MSG)
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_009
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_009", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_009---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback);
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback2);
                            setTimeout(()=>{
                                console.info('------------------Pedometer_DetectionJsTest_009 off in--------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                console.info('------------------Pedometer_DetectionJsTest_009 off end-------------------');
                                done();
                            }, 1000);
                        } catch (err) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_010
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_010", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_010---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback);
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback2);
                            setTimeout(()=>{
                                console.info('----------------Pedometer_DetectionJsTest_010 off in------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION, callback);
                                console.info('----------------Pedometer_DetectionJsTest_010 off end-----------------');
                            }, 500);
                            setTimeout(()=>{
                                console.info('----------------Pedometer_DetectionJsTest_010 off in------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION, callback2);
                                console.info('----------------Pedometer_DetectionJsTest_010 off end-----------------');
                                done();
                            }, 1000);
                        } catch (err) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_011
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_011", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_011---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback, {'interval': 100000000});
                            sensor.once(sensor.SensorId.PEDOMETER_DETECTION, callback2);
                            setTimeout(()=>{
                                console.info('----------------Pedometer_DetectionJsTest_011 off in------------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                console.info('----------------Pedometer_DetectionJsTest_011 off end-----------------');
                                done();
                            }, 1000);
                        } catch (err) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_012
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_012", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_012---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback, {'interval': 100000000});
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback2, {'interval': 100000000});
                            setTimeout(()=>{
                                console.info('-------------Pedometer_DetectionJsTest_012 off in-----------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION, callback);
                                console.info('-------------Pedometer_DetectionJsTest_012 off end----------------');
                            }, 500);
                            setTimeout(()=>{
                                console.info('-------------Pedometer_DetectionJsTest_012 off in-----------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION, callback2);
                                console.info('-------------Pedometer_DetectionJsTest_012 off end----------------');
                                done();
                            }, 1000);
                        } catch (err) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })

    /*
    * @tc.name:BarometerJsTest_013
    * @tc.desc:verify app info is not null
    * @tc.type: FUNC
    * @tc.require: Issue Number
    */
    it("Pedometer_DetectionJsTest_013", 0, async function (done) {
        console.info('----------------------Pedometer_DetectionJsTest_013---------------------------');
        var atManager = abilityAccessCtrl.createAtManager();
        await atManager.grantUserGrantedPermission(tokenID, permissionsNameUser, permissionsFlag.PERMISSION_SYSTEM_FIXED)
        .then(() => {
            try {
                sensor.getSingleSensor(sensor.SensorId.PEDOMETER_DETECTION, (err, data) => {
                    if(err) {
                        console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                        expect(false).assertTrue();
                        done();
                    }
                    expect(typeof(data)).assertEqual("object");
                    if(data) {            
                        try {
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback, {'interval': 100000000});
                            sensor.on(sensor.SensorId.PEDOMETER_DETECTION, callback2, {'interval': 100000000});
                            setTimeout(()=>{
                                console.info('--------------Pedometer_DetectionJsTest_013 off in-----------------');
                                sensor.off(sensor.SensorId.PEDOMETER_DETECTION);
                                console.info('--------------Pedometer_DetectionJsTest_013 off end----------------');
                                done();
                            }, 1000);
                        } catch (err) {
                            console.error('On fail, errCode:' + err.code + ' ,msg:' + err.message);
                            expect(false).assertTrue();
                            done();
                        }
                    } else {
                        console.info('Non-mobile devices cannot be excluded');
                        expect(true).assertTrue();
                        done();
                    }
                });
            } catch (err) {
                console.info('getSingleSensor fail, errCode:' + err.code + ' ,msg:' + err.message);
                expect(err.code).assertEqual(PARAMETER_ERROR_CODE);
                expect(err.message).assertEqual(PARAMETER_ERROR_MSG);
                done();
            }
        })
        .catch ((err) => {
            console.error('err:' + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_NOT_HAVE_PERMISSION);
            done();
        })
    })
})