/*
 * Copyright (c) 2022  Huawei Device Co., Ltd.
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

import { AsyncCallback , Callback} from './basic';

/**
 * Provides methods for managing bundle usage statistics,
 * including the methods for querying bundle usage information and state data.
 *
 * <p>You can use the methods defined in this class to query
 * the usage history and states of bundles in a specified period.
 * The system stores the query result in a {@link BundleStatsInfo} instance and
 * then returns it to you.
 *
 * @since 9
 */
declare namespace usageStatistics {

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
    interface BundleStatsInfo {
        /**
         * The identifier of BundleStatsInfo.
         */
        id: number;
        /**
         * The total duration, in milliseconds.
         */
        abilityInFgTotalTime?: number;
        /**
         * The last time when the application was accessed, in milliseconds.
         */
        abilityPrevAccessTime?: number;
        /**
         * The last time when the application was visible in the foreground, in milliseconds.
         */
        abilityPrevSeenTime?: number;
        /**
         * The total duration, in milliseconds.
         */
        abilitySeenTotalTime?: number;
        /**
         * The bundle name of the application.
         */
        bundleName?: string;
        /**
         * The total duration, in milliseconds.
         */
        fgAbilityAccessTotalTime?: number;
        /**
         * The last time when the foreground application was accessed, in milliseconds.
         */
        fgAbilityPrevAccessTime?: number;
        /**
         * The time of the first bundle usage record in this {@code BundleActiveInfo} object,
         * in milliseconds.
         */
        infosBeginTime?: number;
        /**
         * The time of the last bundle usage record in this {@code BundleActiveInfo} object,
         * in milliseconds.
         */
        infosEndTime?: number;
    }

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
    interface HapFormInfo {
        /**
         * The form name.
         */
        formName: string;
        /**
         * The form dimension.
         */
        formDimension: number;
        /**
         * The form id.
         */
        formId: number;
        /**
         * The last time when the form was accessed, in milliseconds..
         */
        formLastUsedTime: number;
        /**
         * The click count of module.
         */
        count: number;
    }

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
    interface HapModuleInfo {
        /**
         * The device id of module.
         */
        deviceId?: string;
        /**
         * The bundle name.
         */
        bundleName: string;
        /**
         * The module name.
         */
        moduleName: string;
        /**
         * The main ability name of module.
         */
        abilityName?: string;
        /**
         * The label id of application.
         */
        appLabelId?: number;
        /**
         * The label id of module.
         */
        labelId?: number;
        /**
         * The description id of application.
         */
        descriptionId?: number;
        /**
         * The ability id of main ability.
         */
        abilityLableId?: number;
        /**
         * The description id of main ability.
         */
        abilityDescriptionId?: number;
        /**
         * The icon id of main ability.
         */
        abilityIconId?: number;
        /**
         * The launch count of module.
         */
        launchedCount: number;
        /**
         * The last time when the module was accessed, in milliseconds.
         */
        lastModuleUsedTime: number;
        /**
         * The form usage record list of current module.
         */
        formRecords: Array<HapFormInfo>;
    }

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
    interface DeviceEventStats {
        /**
         * The bundle name or system event name.
         */
        name: string;

        /**
         * The event id.
         */
        eventId: number;

        /**
         * The the event occurrence number.
         */
        count: number;
    }

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     */
     interface BundleEvents {
        /**
         * The usage group of the application.
         */
        appGroup?: number;
        /**
         * The bundle name.
         */
        bundleName?: string;
        /**
         * The shortcut ID.
         */
        indexOfLink?: string;
        /**
         * The class name.
         */
        nameOfClass?: string;
        /**
         * The time when this state occurred, in milliseconds.
         */
        eventOccurredTime?: number;
        /**
         * The event id.
         */
        eventId?: number;
    }
    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @systemapi Hide this for inner system use.
     */
    interface AppGroupCallbackInfo {
        /*
         * The usage old group of the application
         */
        appOldGroup: number;
        /*
         * The usage new group of the application
         */
        appNewGroup: number;
        /*
         * The use id
         */
        userId: number;
        /*
         * The change reason
         */
        changeReason: number;
        /*
         * The bundle name
         */
        bundleName: string;
    }

    /**
     * Checks whether the application with a specified bundle name is in the idle state.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @param bundleName Indicates the bundle name of the application to query.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @return Returns {@code true} if the application is idle in a particular period;
     * returns {@code false} otherwise. The time range of the particular period is defined by the system,
     * which may be hours or days.
     */
    function isIdleState(bundleName: string, callback: AsyncCallback<boolean>): void;
    function isIdleState(bundleName: string): Promise<boolean>;

    /**
     * Queries the app group of the calling application.
     *
     * <p>The priority defined in a priority group restricts the resource usage of an application,
     * for example, restricting the running of background tasks. </p>
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000005 - Application is not installed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10100002 - Get Application group info failed.
     * @return Returns the app group of the calling application.
     */
    function queryAppGroup(callback: AsyncCallback<number>): void;
    function queryAppGroup(): Promise<number>;

    /**
     * Queries the usage priority group of the calling application.
     *
     * <p>The priority defined in a priority group restricts the resource usage of an application,
     * for example, restricting the running of background tasks. </p>
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param bundleName, name of the application.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000005 - Application is not installed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10100002 - Get Application group info failed.
     * @return Returns the usage priority group of the calling application.
     */
     function queryAppGroup(bundleName : string, callback: AsyncCallback<number>): void;
     function queryAppGroup(bundleName : string): Promise<number>;

    /**
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
     interface BundleStatsMap {
        [key: string]: BundleStatsInfo;
    }

    /**
     * Queries usage information about each bundle within a specified period.
     *
     * <p>This method queries usage information at the {@link #BY_OPTIMIZED} interval by default.</p>
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link BundleStatsMap} objects containing the usage information about each bundle.
     */
    function queryBundleStatsInfos(begin: number, end: number, callback: AsyncCallback<BundleStatsMap>): void;
    function queryBundleStatsInfos(begin: number, end: number): Promise<BundleStatsMap>;

    /**
     * Declares interval type.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @systemapi Hide this for inner system use.
     */
    export enum IntervalType {
        /**
         * Indicates the interval type that will determine the optimal interval based on the start and end time.
         */
        BY_OPTIMIZED = 0,

        /**
         * Indicates the daily interval.
         */
        BY_DAILY = 1,

        /**
         * Indicates the weekly interval.
         */
        BY_WEEKLY = 2,

        /**
         * Indicates the monthly interval.
         */
        BY_MONTHLY = 3,

        /**
         * Indicates the annually interval.
         */
        BY_ANNUALLY = 4
    }

    /**
     * Queries usage information about each bundle within a specified period at a specified interval.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param byInterval Indicates the interval at which the usage statistics are queried.
     * The value can be {@link #BY_OPTIMIZED}, {@link #BY_DAILY},
     * {@link #BY_WEEKLY}, {@link #BY_MONTHLY}, or {@link #BY_ANNUALLY}.
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the list of {@link BundleStatsInfo} objects containing the usage information about each bundle.
     */
    function queryBundleStatsInfoByInterval(byInterval: IntervalType, begin: number, end: number, callback: AsyncCallback<Array<BundleStatsInfo>>): void;
    function queryBundleStatsInfoByInterval(byInterval: IntervalType, begin: number, end: number): Promise<Array<BundleStatsInfo>>;

    /**
     * Queries state data of all bundles within a specified period identified by the start and end time.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the list of {@link BundleEvents} objects containing the state data of all bundles.
     */
    function queryBundleEvents(begin: number, end: number, callback: AsyncCallback<Array<BundleEvents>>): void;
    function queryBundleEvents(begin: number, end: number): Promise<Array<BundleEvents>>;

    /**
     * Queries state data of the current bundle within a specified period.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link BundleEvents} object Array containing the state data of the current bundle.
     */
    function queryCurrentBundleEvents(begin: number, end: number, callback: AsyncCallback<Array<BundleEvents>>): void;
    function queryCurrentBundleEvents(begin: number, end: number): Promise<Array<BundleEvents>>;

    /**
     * Queries recently module usage records.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param maxNum Indicates max record number in result, max value is 1000, default value is 1000.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link HapModuleInfo} object Array containing the usage data of the modules.
     */
    function queryModuleUsageRecords(maxNum: number, callback: AsyncCallback<Array<HapModuleInfo>>): void;
    function queryModuleUsageRecords(maxNum: number): Promise<Array<HapModuleInfo>>;

    /**
     * Queries recently module usage records.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link HapModuleInfo} object Array containing the usage data of the modules.
     */
    function queryModuleUsageRecords(callback: AsyncCallback<Array<HapModuleInfo>>): void;
    function queryModuleUsageRecords(): Promise<Array<HapModuleInfo>>;

     /**
     * Declares group type.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @systemapi Hide this for inner system use.
     */
      export enum GroupType {
        /**
         * Indicates the alive group.
         */
        ALIVE_GROUP = 10,

        /**
         * Indicates the daily group.
         */
        DAILY_GROUP = 20,

        /**
         * Indicates the fixed group.
         */
        FIXED_GROUP = 30,

        /**
         * Indicates the rare group.
         */
        RARE_GROUP = 40,

        /**
         * Indicates the limit group.
         */
         LIMITED_GROUP = 50,

         /**
         * Indicates the never group.
         */
         NEVER_GROUP = 60
    }

    /**
     * Set app group by bundleName and number.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param bundleName, name of the application.
     * @param newGroup, the group of the application whose name is bundleName.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10100001 - Application group operation repeated.
     */
    function setAppGroup(bundleName: string, newGroup: GroupType, callback: AsyncCallback<void>): void;
    function setAppGroup(bundleName: string, newGroup: GroupType): Promise<void>;

    /**
     * Register callback to service.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param Callback<AppGroupCallbackInfo>, callback when application group change,return the AppGroupCallbackInfo.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10100001 - Application group operation repeated.
     * @return Returns AppGroupCallbackInfo when the group of bundle changed.
     */
    function registerAppGroupCallBack(groupCallback: Callback<AppGroupCallbackInfo>, callback: AsyncCallback<void>): void;
    function registerAppGroupCallBack(groupCallback: Callback<AppGroupCallbackInfo>): Promise<void>;

    /**
     * Unregister callback from service.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10100001 - Application group operation repeated.
     */
    function unregisterAppGroupCallBack(callback: AsyncCallback<void>): void;
    function unregisterAppGroupCallBack(): Promise<void>;

    /** 
     * Queries device event states data within a specified period identified by the start and end time.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link DeviceEventStats} object Array containing the event states data.
     */
    function queryDeviceEventStats(begin: number, end: number, callback: AsyncCallback<Array<DeviceEventStats>>): void;
    function queryDeviceEventStats(begin: number, end: number): Promise<Array<DeviceEventStats>>;

    /**
     * Queries app notification number within a specified period identified by the start and end time.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @permission ohos.permission.BUNDLE_ACTIVE_INFO
     * @systemapi Hide this for inner system use.
     * @param begin Indicates the start time of the query period, in milliseconds.
     * @param end Indicates the end time of the query period, in milliseconds.
     * @throws { BusinessError } 201 - Parameter error.
     * @throws { BusinessError } 401 - Permission denied.
     * @throws { BusinessError } 801 - Capability not supported.
     * @throws { BusinessError } 10000001 - Memory operation failed.
     * @throws { BusinessError } 10000002 - Parcel operation failed.
     * @throws { BusinessError } 10000003 - System service operation failed.
     * @throws { BusinessError } 10000004 - IPC Communication failed.
     * @throws { BusinessError } 10000006 - Get application info failed.
     * @throws { BusinessError } 10000007 - Get system or actual time failed.
     * @return Returns the {@link NotificationEventStats} object Array containing the event states data.
     */
    function queryNotificationEventStats(begin: number, end: number, callback: AsyncCallback<Array<DeviceEventStats>>): void;
    function queryNotificationEventStats(begin: number, end: number): Promise<Array<DeviceEventStats>>;
}

export default usageStatistics;