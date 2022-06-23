/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import bundleState from '@ohos.bundleState'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("DeviceUsageStatisticsJsTest", function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll called')
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('afterAll called')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('beforeEach called')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach caled')
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest001
     * @tc.desc: test isIdleState promise.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89H AR000GH89I AR000GH899
     */
    it("DeviceUsageStatisticsJsTest001", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest001---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        bundleState.isIdleState(bundleName).then((res) => {
            console.info('BUNDLE_ACTIVE isIdleState promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE isIdleState promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest002
     * @tc.desc: test isIdleState callback.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89E AR000GH89F AR000GH89G
     */
    it("DeviceUsageStatisticsJsTest002", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest002---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        bundleState.isIdleState(bundleName, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE isIdleState callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE isIdleState callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest003
     * @tc.desc: test queryBundleActiveStates promise.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89H AR000GH89I AR000GH899
     */
    it("DeviceUsageStatisticsJsTest003", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest003---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveStates promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveStates promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest004
     * @tc.desc: test queryBundleActiveStates callback.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89E AR000GH89F AR000GH89G
     */
    it("DeviceUsageStatisticsJsTest004", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest004---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryBundleActiveStates callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryBundleActiveStates callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest005
     * @tc.desc: test queryBundleStateInfos promise.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89H AR000GH89I AR000GH899
     */
    it("DeviceUsageStatisticsJsTest005", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest005---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfos(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfos promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfos promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest006
     * @tc.desc: test queryBundleStateInfos callback.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89E AR000GH89F AR000GH89G
     */
    it("DeviceUsageStatisticsJsTest006", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest006---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfos(beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryBundleStateInfos callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryBundleStateInfos callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest007
     * @tc.desc: test queryCurrentBundleActiveStates promise.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89H AR000GH89I AR000GH899
     */
    it("DeviceUsageStatisticsJsTest007", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest007---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryCurrentBundleActiveStates(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest008
     * @tc.desc: test queryCurrentBundleActiveStates callback.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89E AR000GH89F AR000GH89G
     */
    it("DeviceUsageStatisticsJsTest008", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest008---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryCurrentBundleActiveStates(beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest009
     * @tc.desc: test queryBundleStateInfoByInterval promise.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89H AR000GH89I AR000GH899
     */
    it("DeviceUsageStatisticsJsTest009", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest009---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest010
     * @tc.desc: test queryBundleStateInfoByInterval callback.
     * @tc.type: FUNC
     * @tc.require: SR000GGTN7 AR000GH89E AR000GH89F AR000GH89G
     */
    it("DeviceUsageStatisticsJsTest010", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest010---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest011
     * @tc.desc: test getRecentlyUsedModules callback.
     * @tc.type: FUNC
     * @tc.require: SR000GU2UE AR0003GU3EQ
     */
    it("DeviceUsageStatisticsJsTest011", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest011---------------------------');
        let maxNum = 1;
        bundleState.getRecentlyUsedModules(maxNum, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE getRecentlyUsedModules callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE getRecentlyUsedModules callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest012
     * @tc.desc: test getRecentlyUsedModules promise.
     * @tc.type: FUNC
     * @tc.require: SR000GU2UE AR0003GU3EQ
     */
    it("DeviceUsageStatisticsJsTest012", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest012---------------------------');
        let maxNum = 1;
        bundleState.getRecentlyUsedModules(maxNum).then((res) => {
            console.info('BUNDLE_ACTIVE getRecentlyUsedModules promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE getRecentlyUsedModules promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest013
     * @tc.desc: test queryAppUsagePriorityGroup promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest013", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest013---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        bundleState.queryAppUsagePriorityGroup(bundleName).then( res => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise success.');
            expect(true).assertEqual(true);
        }).catch( err => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest014
     * @tc.desc: test queryAppUsagePriorityGroup callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest014", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest014---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        bundleState.queryAppUsagePriorityGroup(bundleName, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest015
     * @tc.desc: test queryAppUsagePriorityGroup promise, with no param.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest015", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest015---------------------------');
        bundleState.queryAppUsagePriorityGroup().then( res => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise success.');
            expect(true).assertEqual(true);
        }).catch( err => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest016
     * @tc.desc: test queryAppUsagePriorityGroup callback, with no param.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest016", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest016---------------------------');
        bundleState.queryAppUsagePriorityGroup((err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest017
     * @tc.desc: test setBundleGroup promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest017", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest017---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        let newGroup   = 30;
        bundleState.setBundleGroup(bundleName, newGroup).then( res => {
            console.info('BUNDLE_ACTIVE setBundleGroup promise success.');
            expect(false).assertEqual(true);
        }).catch( err => {
            console.info('BUNDLE_ACTIVE setBundleGroup promise failure.');
            expect(true).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest018
     * @tc.desc: test setBundleGroup callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest018", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest018---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        let newGroup   = 30;
        bundleState.setBundleGroup(bundleName, newGroup, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE setBundleGroup callback failure.');
                expect(true).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE setBundleGroup callback success.');
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest019
     * @tc.desc: test registerGroupCallBack promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest019", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest019---------------------------');
        let onBundleGroupChanged = (err,res) =>{
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack callback success.');
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack appUsageOldGroup is : ' + res.appUsageOldGroup);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack appUsageNewGroup is : ' + res.appUsageNewGroup);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack changeReason is : ' + res.changeReason);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack userId is : ' + res.userId);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack bundleName is : ' + res.bundleName);
        };
        bundleState.registerGroupCallBack(onBundleGroupChanged).then( res => {
            console.info('BUNDLE_ACTIVE RegisterGroupCallBack promise success.');
            expect(true).assertEqual(true);
        }).catch( err => {
            console.info('BUNDLE_ACTIVE RegisterGroupCallBack promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest020
     * @tc.desc: test registerGroupCallBack callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest020", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest020---------------------------');
        let onBundleGroupChanged = (err,res) =>{
            console.log('BUNDLE_ACTIVE onBundleGroupChanged RegisterGroupCallBack callback success.');
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack appUsageOldGroup is : ' + res.appUsageOldGroup);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack appUsageNewGroup is : ' + res.appUsageNewGroup);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack changeReason is : ' + res.changeReason);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack userId is : ' + res.userId);
            console.log('BUNDLE_ACTIVE RegisterGroupCallBack bundleName is : ' + res.bundleName);
        };
        bundleState.registerGroupCallBack(onBundleGroupChanged, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE registerGroupCallBack callback failure.');
                expect(true).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE registerGroupCallBack callback success.');
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest021
     * @tc.desc: test unRegisterGroupCallBack promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest021", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest021---------------------------');
        bundleState.unRegisterGroupCallBack().then( res => {
            console.info('BUNDLE_ACTIVE unRegisterGroupCallBack promise success.');
            expect(true).assertEqual(true);
        }).catch( err => {
            console.info('BUNDLE_ACTIVE unRegisterGroupCallBack promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest022
     * @tc.desc: test unRegisterGroupCallBack callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest022", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest022---------------------------');
        bundleState.unRegisterGroupCallBack((err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE unRegisterGroupCallBack callback failure.');
                expect(true).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE unRegisterGroupCallBack callback success.');
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest023
     * @tc.desc: test queryBundleActiveEventStates promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0H9H AR000H0ROG
     */
    it("DeviceUsageStatisticsJsTest023", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest023---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveEventStates(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveEventStates promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveEventStates promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest024
     * @tc.desc: test queryBundleActiveEventStates callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0H9H AR000H0ROG
     */
    it("DeviceUsageStatisticsJsTest024", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest024---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveEventStates(beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryBundleActiveEventStates callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryBundleActiveEventStates callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest025
     * @tc.desc: test queryAppNotificationNumber promise.
     * @tc.type: FUNC
     * @tc.require: SR000H0H7D AR000H0RR6
     */
    it("DeviceUsageStatisticsJsTest025", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest025---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryAppNotificationNumber(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryAppNotificationNumber promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryAppNotificationNumber promise failure.');
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest026
     * @tc.desc: test queryAppNotificationNumber callback.
     * @tc.type: FUNC
     * @tc.require: SR000H0H7D AR000H0RR6
     */
    it("DeviceUsageStatisticsJsTest026", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest026---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryAppNotificationNumber(beginTime, endTime, (err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryAppNotificationNumber callback failure.');
                expect(false).assertEqual(true);
            } else {
                console.info('BUNDLE_ACTIVE queryAppNotificationNumber callback success.');
                expect(true).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
})

