/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import sensor from '@ohos.sensor'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("SensorJsTest", function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll caled')
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
    })

    // function timeout(done) {
    //     expect().assertFail();
    //     console.debug('SensorJsTest=========timeout========');
    //     done();
    // }

    /*
     * @tc.name:SensorJsTest001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest001", 0, async function (done) {
        console.info('----------------------SensorJsTest001---------------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                console.info('setTimeout ..start')
                setTimeout(()=>{
                    sensor.off(0, (error) =>{
                        if(error) {
                            console.info('SensorJsTest001 HYH_TEST off1  error');
                            expect(false).assertTrue();
                            reject();
                        } else {
                            console.info('SensorJsTest001 HYH_TEST off1  error');
                            expect(true).assertTrue();
                            resolve()
                        }
                    });
                    console.info('setTimeout ..end')
                }, 1000)
            })
        }

        let promise = new Promise((resolve, reject) => {
            sensor.on(0, function(error, data) {
                if (error) {
                    console.info('SensorJsTest001 HYH_TEST on error');
                    expect(false).assertTrue();
                    reject()
                } else {
                    console.info('SensorJsTest001 HYH_TEST on success');
                    expect(true).assertTrue();
                    resolve()
                }
            });
        })

        await promise.then(() =>{
            return offPromise();
        }, ()=>{
            console.info("SensorJsTest001 reject");
        })
        done();
    })

    /*
     * @tc.name:SensorJsTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest002", 0, async function (done) {
        console.info('----------------------SensorJsTest002---------------------------');
        function onSensorCallback(error, data) {
            if (error) {
                console.info('SensorJsTest002 HYH_TEST on error');
                expect(true).assertTrue();
            } else {
                console.info('SensorJsTest002 HYH_TEST on success' + data.x);
                expect(false).assertTrue();
            }
            done();
        }
        sensor.on(-1, onSensorCallback);
    })

    /*
     * @tc.name:SensorJsTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest003", 0, async function (done) {
        console.info('----------------------SensorJsTest003---------------------------');
        let promise = new Promise((resolve,reject) => {
            sensor.on(0, function(error, data){
                if (error) {
                    console.info('SensorJsTest003 HYH_TEST on error');
                    expect(false).assertTrue();
                    reject()
                } else {
                    console.info('SensorJsTest003 HYH_TEST on success' + data.x);
                    expect(true).assertTrue();
                    resolve()
                }
            }, {'interval': 100000000});
        })
        function offPromise() {
            new Promise((resolve, reject) => {
                console.info('setTimeout ..start')
                setTimeout(()=>{
                    sensor.off(0, (error) =>{
                        if(error) {
                            console.info('SensorJsTest003 HYH_TEST off1  error');
                            expect(false).assertTrue();
                            reject();
                        } else {
                            console.info('SensorJsTest003 HYH_TEST off1  error');
                            expect(true).assertTrue();
                            resolve()
                        }
                    });
                    console.info('setTimeout ..end')
                }, 1000)
            })
        } 
        await promise.then(() =>{
            return offPromise();
        })
        done()
    })

    /*
     * @tc.name:SensorJsTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest004", 0, function () {
        sensor.on(0, function(){}, {'interval': 100000000}, 5);
        expect(true).assertTrue();
    })

    /*
     * @tc.name:SensorJsTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest005", 0, async function (done) {
        let promise1 = new Promise((resolve,reject) => {
            sensor.on(0, function(error, data){
                if (error) {
                    console.info('SensorJsTest005 HYH_TEST on1 error');
                    expect(false).assertTrue();
                    reject()
                } else {
                    console.info('SensorJsTest005 HYH_TEST on1 success' + data.x);
                    expect(true).assertTrue();
                    resolve()
                }
            });
        })
        function promise2() {
            new Promise((resolve,reject) => {
                setTimeout(()=>{
                    sensor.on(0, function(error, data){
                        if (error) {
                            console.info('SensorJsTest005 HYH_TEST on2 error');
                            expect(false).assertTrue();
                            reject()
                        } else {
                            console.info('SensorJsTest005 HYH_TEST on2 success' + data.x);
                            expect(true).assertTrue();
                            resolve()
                        }
                    });
                }, 1000)
            })
        }
        function offPromise() {
            new Promise((resolve,reject) => {
                console.info('setTimeout ..start')
                setTimeout(()=>{
                    sensor.off(0, (error) =>{
                        if(error) {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(false).assertTrue();
                            reject();
                        } else {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(true).assertTrue();
                            resolve()
                        }
                    });
                    console.info('setTimeout ..end')
                }, 1000)
            })
        }
        await promise1.then(() =>{
            return promise2();
        }).then(()=>{
            return offPromise();
        })
        done();
    })

    /*
     * @tc.name:SensorJsTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest006", 0, async function (done) {
        let promise1 = new Promise((resolve,reject) => {
            sensor.on(0, function(error, data){
                if (error) {
                    console.info('SensorJsTest006 HYH_TEST on1 error');
                    expect(false).assertTrue();
                    reject()
                } else {
                    console.info('SensorJsTest006 HYH_TEST on1 success' + data.x);
                    expect(true).assertTrue();
                    resolve()
                }
            });
        })
        function offPromise() {
            new Promise((resolve,reject) => {
                console.info('setTimeout ..start')
                setTimeout(()=>{
                    sensor.off(0, (error) =>{
                        if(error) {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(false).assertTrue();
                            reject();
                        } else {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(true).assertTrue();
                            resolve()
                        }
                    });
                    console.info('setTimeout ..end')
                }, 1000)
            })
        }

        function offPromise2() {
            new Promise((resolve,reject) => {
                console.info('setTimeout ..start')
                setTimeout(()=>{
                    sensor.off(0, (error) =>{
                        if(error) {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(true).assertTrue();
                            resolve();
                        } else {
                            console.info('SensorJsTest006 HYH_TEST off1  error');
                            expect(false).assertTrue();
                            reject()
                        }
                    });
                    console.info('setTimeout ..end')
                }, 1000)
            })
        }

        await promise1.then(() =>{
            return offPromise();
        }).then(()=>{
            return offPromise2();
        })
        done();
    })

    /*
     * @tc.name:SensorJsTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest007", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorJsTest007 HYH_TEST on error');
                expect(false).assertTrue();
            } else {
                console.info('SensorJsTest007 HYH_TEST on success' + data.x);
                expect(true).assertTrue();
            }
            done();
        }
        sensor.once(0, onceSensorCallback);
    })

    /*
     * @tc.name:SensorJsTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest008", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorJsTest008 HYH_TEST on error');
                expect(true).assertTrue();
            } else {
                console.info('SensorJsTest008 HYH_TEST on success' + data.x);
                expect(false).assertTrue();
            }
            done();
        }
        sensor.once(-1, onceSensorCallback);
    })

    /*
     * @tc.name:SensorJsTest009
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest009", 0, async function (done) {
        let onPromise = new Promise((resolve,reject) => {
            sensor.on(0, function(error, data){
                if (error) {
                    console.info('SensorJsTest009 HYH_TEST on1 error');
                    expect(false).assertTrue();
                    reject()
                } else {
                    console.info('SensorJsTest009 HYH_TEST on1 success' + data.x);
                    expect(true).assertTrue();
                    resolve()
                }
            });
        })
        function oncePromise() {
            new Promise((resolve,reject) => {
                sensor.once(0, (error) =>{
                    if (error) {
                        console.info('SensorJsTest009 HYH_TEST on error');
                        expect(false).assertTrue();
                        reject();
                    } else {
                        console.info('SensorJsTest009 HYH_TEST on success' + data.x);
                        expect(true).assertTrue();
                        resolve();
                    }
                });
            })
        }

        function offPromise() {
            new Promise((resolve,reject) => {
                console.info('setTimeout ..start')
                sensor.off(0, (error) =>{
                    if(error) {
                        console.info('SensorJsTest009 HYH_TEST off1  error');
                        expect(false).assertTrue();
                        reject();
                    } else {
                        console.info('SensorJsTest009 HYH_TEST off1  error');
                        expect(true).assertTrue();
                        resolve()
                    }
                });
                console.info('setTimeout ..end')
            })
        }

        await onPromise.then(() =>{
            return oncePromise();
        }).then(()=>{
            return offPromise();
        })
        done();
    })

    /*
     * @tc.name:SensorJsTest010
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest010", 0, function () {
        sensor.once(0, function(){}, 5);
        expect(true).assertTrue();
    })

    /*
     * @tc.name:SensorJsTest011
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest011", 0, async function (done) {
        function offCallback(error) {
            if (error) {
                console.info('SensorJsTest011 HYH_TEST on error');
                expect(true).assertTrue();
            } else {
                console.info('SensorJsTest011 HYH_TEST on success' + data.x);
                expect(false).assertTrue();
            }
            done();
        }
        sensor.off(-1, offCallback);
    })

    /*
     * @tc.name:SensorJsTest012
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SensorJsTest012", 0, async function (done) {
        function offCallback(error) {
            if (error) {
                console.info('SensorJsTest012 HYH_TEST on error');
                expect(false).assertTrue();
            } else {
                console.info('SensorJsTest012 HYH_TEST on success' + data.x);
                expect(true).assertTrue();
            }
            done();
        }
        sensor.off(0, offCallback);
    })

    /*
     * @tc.name:SensorJsTest013
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
        it("SensorJsTest013", 0, function () {
            sensor.off(0, function(){}, 5);
            expect(true).assertTrue();
        })
})
