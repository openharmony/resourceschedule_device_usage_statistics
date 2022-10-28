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
import usageStatistics from '@ohos.resourceschedule.usageStatistics'

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
            done();
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE isIdleState promise failure.');
            expect(false).assertEqual(true);
            done();
        });
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
                done();
            } else {
                console.info('BUNDLE_ACTIVE isIdleState callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
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
            done();
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveStates promise failure.');
            expect(false).assertEqual(true);
            done();
        });
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
                done();
            } else {
                console.info('BUNDLE_ACTIVE queryBundleActiveStates callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
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
            done();
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfos promise failure.');
            expect(false).assertEqual(true);
            done();
        });
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
                done();
            } else {
                console.info('BUNDLE_ACTIVE queryBundleStateInfos callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
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
            done();
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates promise failure.');
            expect(false).assertEqual(true);
            done();
        });
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
                done();
            } else {
                console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
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
            done();
        }).catch((err) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval promise failure.');
            expect(false).assertEqual(true);
            done();
        });
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
                done();
            } else {
                console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest011
     * @tc.desc: test queryAppUsagePriorityGroup promise, with no param.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest011", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest011---------------------------');
        bundleState.queryAppUsagePriorityGroup().then( res => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise success.');
            expect(true).assertEqual(true);
            done();
        }).catch( err => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise failure.');
            expect(false).assertEqual(true);
            done();
        });
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest012
     * @tc.desc: test queryAppUsagePriorityGroup callback, with no param.
     * @tc.type: FUNC
     * @tc.require: SR000H0HAQ AR000H0ROE
     */
    it("DeviceUsageStatisticsJsTest012", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest012---------------------------');
        bundleState.queryAppUsagePriorityGroup((err, res) => {
            if (err) {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback failure.');
                expect(false).assertEqual(true);
                done();
            } else {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback success.');
                expect(true).assertEqual(true);
                done();
            }
        });
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest013
     * @tc.desc: test isIdleState callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest013", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest013---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        try{
            usageStatistics.isIdleState(bundleName, (err, res) => {
                if (err) {
                    console.info('BUNDLE_ACTIVE isIdleState callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE isIdleState callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch(error) {
            console.info('BUNDLE_ACTIVE isIdleState callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest014
     * @tc.desc: test isIdleState promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest014", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest014---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        try{
            usageStatistics.isIdleState(bundleName).then((res) => {
                console.info('BUNDLE_ACTIVE isIdleState promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch((err) => {
                console.info('BUNDLE_ACTIVE isIdleState promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE isIdleState promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest015
     * @tc.desc: test queryAppGroup callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest015", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest015---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        try{
            usageStatistics.queryAppGroup(bundleName, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryAppGroup callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryAppGroup callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryAppGroup callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest016
     * @tc.desc: test queryAppGroup promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest016", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest016---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        try{
            usageStatistics.queryAppGroup(bundleName).then( res => {
                console.info('BUNDLE_ACTIVE queryAppGroup promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE queryAppGroup promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryAppGroup promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest017
     * @tc.desc: test queryAppGroup callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest017", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest017---------------------------');
        try{
            usageStatistics.queryAppGroup((err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryAppGroup callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryAppGroup callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryAppGroup callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest018
     * @tc.desc: test queryAppGroup promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest018", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest018---------------------------');
        try{
            usageStatistics.queryAppGroup().then( res => {
                console.info('BUNDLE_ACTIVE queryAppGroup promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE queryAppGroup promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryAppGroup promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest019
     * @tc.desc: test queryBundleStatsInfos callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest019", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest019---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try {
            usageStatistics.queryBundleStatsInfos(beginTime, endTime, (err, res) => {
                if (err) {
                    console.info('BUNDLE_ACTIVE queryBundleStatsInfos callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryBundleStatsInfos callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleStatsInfos callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest020
     * @tc.desc: test queryBundleStatsInfos promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest020", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest020---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryBundleStatsInfos(beginTime, endTime).then((res) => {
                console.info('BUNDLE_ACTIVE queryBundleStatsInfos promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch((err) => {
                console.info('BUNDLE_ACTIVE queryBundleStatsInfos promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleStatsInfos promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest021
     * @tc.desc: test queryBundleStatsInfoByInterval callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest021", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest021---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryBundleStatsInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
                if (err) {
                    console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest022
     * @tc.desc: test queryBundleStatsInfoByInterval promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest022", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest022---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryBundleStatsInfoByInterval(intervalType, beginTime, endTime).then((res) => {
                console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch((err) => {
                console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleStatsInfoByInterval promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest023
     * @tc.desc: test queryBundleEvents callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest023", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest023---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryBundleEvents(beginTime, endTime, (err, res) => {
                if (err) {
                    console.info('BUNDLE_ACTIVE queryBundleEvents callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryBundleEvents callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleEvents callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest024
     * @tc.desc: test queryBundleEvents promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest024", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest024---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryBundleEvents(beginTime, endTime).then((res) => {
                console.info('BUNDLE_ACTIVE queryBundleEvents promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch((err) => {
                console.info('BUNDLE_ACTIVE queryBundleEvents promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryBundleEvents promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest025
     * @tc.desc: test queryCurrentBundleEvents callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest025", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest025---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryCurrentBundleEvents(beginTime, endTime, (err, res) => {
                if (err) {
                    console.info('BUNDLE_ACTIVE queryCurrentBundleEvents callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryCurrentBundleEvents callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryCurrentBundleEvents callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest026
     * @tc.desc: test queryCurrentBundleEvents promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest026", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest026---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryCurrentBundleEvents(beginTime, endTime).then((res) => {
                console.info('BUNDLE_ACTIVE queryCurrentBundleEvents promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch((err) => {
                console.info('BUNDLE_ACTIVE queryCurrentBundleEvents promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryCurrentBundleEvents promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest027
     * @tc.desc: test queryModuleUsageRecords callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest027", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest027---------------------------');
        let maxNum = 1;
        try{
            usageStatistics.queryModuleUsageRecords(maxNum, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest028
     * @tc.desc: test queryModuleUsageRecords promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest028", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest028---------------------------');
        let maxNum = 1;
        try{
            usageStatistics.queryModuleUsageRecords(maxNum).then( res => {
                console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err=> {
                console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise failure.');
                expect(false).assertEqual(true);
                done();
        });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest029
     * @tc.desc: test queryModuleUsageRecords callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest029", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest029---------------------------');
        try{
            usageStatistics.queryModuleUsageRecords((err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryModuleUsageRecords callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest030
     * @tc.desc: test queryModuleUsageRecords promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest030", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest030---------------------------');
        try{
            usageStatistics.queryModuleUsageRecords().then( res => {
                console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err=> {
                console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise failure.');
                expect(false).assertEqual(true);
                done();
        });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryModuleUsageRecords promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest031
     * @tc.desc: test setAppGroup callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest031", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest031---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        let newGroup   = 30;
        try{
            usageStatistics.setAppGroup(bundleName, newGroup, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE setAppGroup callback failure.');
                    expect(true).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE setAppGroup callback success.');
                    expect(false).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE setAppGroup callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest032
     * @tc.desc: test setAppGroup promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest032", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest032---------------------------');
        let bundleName = 'com.example.deviceUsageStatistics';
        let newGroup   = 30;
        try{
            usageStatistics.setAppGroup(bundleName, newGroup).then( res => {
                console.info('BUNDLE_ACTIVE setAppGroup promise success.');
                expect(false).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE setAppGroup promise failure.');
                expect(true).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE setAppGroup promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest033
     * @tc.desc: test registerAppGroupCallBack callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest033", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest033---------------------------');
        let onBundleGroupChanged = (err, res) =>{
            console.log('BUNDLE_ACTIVE onBundleGroupChanged registerAppGroupCallBack callback success.');
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result appUsageOldGroup is : ' + res.appOldGroup);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result appUsageNewGroup is : ' + res.appNewGroup);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result changeReason is : ' + res.changeReason);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result userId is : ' + res.userId);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result bundleName is : ' + res.bundleName);
          };
        try{
            usageStatistics.registerAppGroupCallBack(onBundleGroupChanged, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE registerAppGroupCallBack callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE registerAppGroupCallBack callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE registerAppGroupCallBack callback failure.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest034
     * @tc.desc: test registerAppGroupCallBack promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest034", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest034---------------------------');
        let onBundleGroupChanged = (err, res) =>{
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack registerAppGroupCallBack callback success.');
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result appOldGroup is : ' + res.appOldGroup);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result appNewGroup is : ' + res.appNewGroup);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result changeReason is : ' + res.changeReason);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result userId is : ' + res.userId);
            console.log('BUNDLE_ACTIVE registerAppGroupCallBack result bundleName is : ' + res.bundleName);
        };
        try{
            usageStatistics.registerAppGroupCallBack(onBundleGroupChanged).then( (res) => {
                console.info('BUNDLE_ACTIVE registerAppGroupCallBack promise success.');
                expect(false).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE registerAppGroupCallBack promise failure.');
                expect(true).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE registerAppGroupCallBack promise failure.');
            expect(true).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest035
     * @tc.desc: test unregisterAppGroupCallBack callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest035", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest035---------------------------');
        try{
            usageStatistics.unregisterAppGroupCallBack((err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest036
     * @tc.desc: test unregisterAppGroupCallBack promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest036", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest036---------------------------');
        try{
            usageStatistics.unregisterAppGroupCallBack().then( (res) => {
                console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack promise success.');
                expect(false).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE unregisterAppGroupCallBack promise throw error.');
            expect(true).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest037
     * @tc.desc: test queryDeviceEventStats callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest037", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest037---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryDeviceEventStats(beginTime, endTime, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryDeviceEventStats callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryDeviceEventStats callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryDeviceEventStats callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest038
     * @tc.desc: test queryDeviceEventStats promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest038", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest038---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryDeviceEventStats(beginTime, endTime).then( res => {
                console.info('BUNDLE_ACTIVE queryDeviceEventStats promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err=> {
                console.info('BUNDLE_ACTIVE queryDeviceEventStats promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryDeviceEventStats promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest039
     * @tc.desc: test queryNotificationEventStats callback.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest039", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest039---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryNotificationEventStats(beginTime, endTime, (err, res) => {
                if(err) {
                    console.info('BUNDLE_ACTIVE queryNotificationEventStats callback failure.');
                    expect(false).assertEqual(true);
                    done();
                } else {
                    console.info('BUNDLE_ACTIVE queryNotificationEventStats callback success.');
                    expect(true).assertEqual(true);
                    done();
                }
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryNotificationEventStats callback throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest040
     * @tc.desc: test queryNotificationEventStats promise.
     * @tc.type: FUNC
     * @tc.require: issueI5V2T4
     */
    it("DeviceUsageStatisticsJsTest040", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest040---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        try{
            usageStatistics.queryNotificationEventStats(beginTime, endTime).then( res => {
                console.info('BUNDLE_ACTIVE queryNotificationEventStats promise success.');
                expect(true).assertEqual(true);
                done();
            }).catch( err => {
                console.info('BUNDLE_ACTIVE queryNotificationEventStats promise failure.');
                expect(false).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info('BUNDLE_ACTIVE queryNotificationEventStats promise throw error.');
            expect(false).assertEqual(true);
            done();
        }
    })
})

