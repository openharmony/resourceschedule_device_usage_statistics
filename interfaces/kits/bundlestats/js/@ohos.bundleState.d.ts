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

import { AsyncCallback, Callback } from './@ohos.base';

/**
 * Provides methods for managing bundle usage statistics,
 * including the methods for querying bundle usage information and state data.
 * <p>You can use the methods defined in this class to query
 * the usage history and states of bundles in a specified period.
 * The system stores the query result in a {@link BundleStateInfo} or {@link BundleActiveState} instance and
 * then returns it to you.
 *
 * @namespace bundleState
 * @since 7
 * @deprecated since 9
 * @useinstead @ohos.resourceschedule.usageStatistics
 */
declare namespace bundleState {
  /**
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  interface BundleStateInfo {
    /**
     * the identifier of BundleStateInfo.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    id: number;

    /**
     * the total duration, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    abilityInFgTotalTime?: number;

    /**
     * the last time when the application was accessed, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    abilityPrevAccessTime?: number;

    /**
     * the last time when the application was visible in the foreground, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    abilityPrevSeenTime?: number;

    /**
     * the total duration, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    abilitySeenTotalTime?: number;

    /**
     * the bundle name of the application.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    bundleName?: string;

    /**
     * the total duration, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    fgAbilityAccessTotalTime?: number;

    /**
     * the last time when the foreground application was accessed, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    fgAbilityPrevAccessTime?: number;

    /**
     * the time of the first bundle usage record in this {@code BundleActiveInfo} object,
     * in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    infosBeginTime?: number;

    /**
     * the time of the last bundle usage record in this {@code BundleActiveInfo} object,
     * in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    infosEndTime?: number;

    /**
     * Merges a specified {@link BundleActiveInfo} object with this {@link BundleActiveInfo} object.
     * The bundle name of both objects must be the same.
     *
     * @param toMerge Indicates the {@link BundleActiveInfo} object to merge.
     * if the bundle names of the two {@link BundleActiveInfo} objects are different.
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @since 7
     * @deprecated since 9
     */
    merge(toMerge: BundleStateInfo): void;
  }

  /**
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  interface BundleActiveState {
    /**
     * the usage priority group of the application.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    appUsagePriorityGroup?: number;

    /**
     * the bundle name.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    bundleName?: string;

    /**
     * the shortcut ID.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    indexOfLink?: string;

    /**
     * the class name.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    nameOfClass?: string;

    /**
     * the time when this state occurred, in milliseconds.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    stateOccurredTime?: number;

    /**
     * the state type.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    stateType?: number;
  }

  /**
   * Checks whether the application with a specified bundle name is in the idle state.
   *
   * @param bundleName Indicates the bundle name of the application to query.
   * @param { AsyncCallback<boolean> } callback - the callback of isIdleState.
   * <p> boolean value is true mean the application is idle in a particular period; false mean otherwise.
   * The time range of the particular period is defined by the system, which may be hours or days.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function isIdleState(bundleName: string, callback: AsyncCallback<boolean>): void;

  /**
   * Checks whether the application with a specified bundle name is in the idle state.
   *
   * @param bundleName Indicates the bundle name of the application to query.
   * @returns { Promise<boolean> } the promise returned by isIdleState.
   * <p> boolean value is true mean the application is idle in a particular period; false mean otherwise.
   * The time range of the particular period is defined by the system, which may be hours or days.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function isIdleState(bundleName: string): Promise<boolean>;

  /**
   * Queries the usage priority group of the calling application.
   * <p>The priority defined in a priority group restricts the resource usage of an application,
   * for example, restricting the running of background tasks. </p>
   *
   * @param { AsyncCallback<number> } callback - the callback of queryAppUsagePriorityGroup.
   * <p> Returns the app group of the calling application.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryAppUsagePriorityGroup(callback: AsyncCallback<number>): void;

  /**
   * Queries the usage priority group of the calling application.
   * <p>The priority defined in a priority group restricts the resource usage of an application,
   * for example, restricting the running of background tasks. </p>
   *
   * @returns { Promise<number> } the promise returned by queryAppUsagePriorityGroup.
   * <p> Returns the app group of the calling application.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.AppGroup
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryAppUsagePriorityGroup(): Promise<number>;

  /**
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  interface BundleActiveInfoResponse {
    [key: string]: BundleStateInfo;
  }

  /**
   * Queries usage information about each bundle within a specified period.
   * <p>This method queries usage information at the {@link #BY_OPTIMIZED} interval by default.</p>
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @param { AsyncCallback<BundleActiveInfoResponse> } callback - the callback of queryBundleStateInfos.
   * <p> the {@link BundleActiveInfoResponse} objects containing the usage information about each bundle.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleStateInfos(begin: number, end: number, callback: AsyncCallback<BundleActiveInfoResponse>): void;

  /**
   * Queries usage information about each bundle within a specified period.
   * <p>This method queries usage information at the {@link #BY_OPTIMIZED} interval by default.</p>
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @returns { Promise<BundleActiveInfoResponse> } the promise returned by queryBundleStatsInfos.
   * <p> the {@link BundleActiveInfoResponse} objects containing the usage information about each bundle.</p>
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleStateInfos(begin: number, end: number): Promise<BundleActiveInfoResponse>;

  /**
   * Declares interval type.
   *
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  export enum IntervalType {
    /**
     * Indicates the interval type that will determine the optimal interval based on the start and end time.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    BY_OPTIMIZED = 0,

    /**
     * Indicates the daily interval.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    BY_DAILY = 1,

    /**
     * Indicates the weekly interval.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    BY_WEEKLY = 2,

    /**
     * Indicates the monthly interval.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    BY_MONTHLY = 3,

    /**
     * Indicates the annually interval.
     *
     * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
     * @deprecated since 9
     */
    BY_ANNUALLY = 4
  }

  /**
   * Queries usage information about each bundle within a specified period at a specified interval.
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param byInterval Indicates the interval at which the usage statistics are queried.
   * <p><p>The value can be {@link #BY_OPTIMIZED}, {@link #BY_DAILY},
   * {@link #BY_WEEKLY}, {@link #BY_MONTHLY}, or {@link #BY_ANNUALLY}.</p>
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @param { AsyncCallback<Array<BundleStateInfo>> } callback - the callback of usage information about each bundle.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleStateInfoByInterval(
    byInterval: IntervalType,
    begin: number,
    end: number,
    callback: AsyncCallback<Array<BundleStateInfo>>
  ): void;

  /**
   * Queries usage information about each bundle within a specified period at a specified interval.
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param byInterval Indicates the interval at which the usage statistics are queried.
   * <p>The value can be {@link #BY_OPTIMIZED}, {@link #BY_DAILY},
   * {@link #BY_WEEKLY}, {@link #BY_MONTHLY}, or {@link #BY_ANNUALLY}.</p>
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @returns { Promise<Array<BundleStateInfo>> } the usage information about each bundle.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleStateInfoByInterval(
    byInterval: IntervalType,
    begin: number,
    end: number
  ): Promise<Array<BundleStateInfo>>;

  /**
   * Queries state data of all bundles within a specified period identified by the start and end time.
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @param { AsyncCallback<Array<BundleActiveState>> } callback - the state data of all bundles.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleActiveStates(begin: number, end: number, callback: AsyncCallback<Array<BundleActiveState>>): void;

  /**
   * Queries state data of all bundles within a specified period identified by the start and end time.
   *
   * @permission ohos.permission.BUNDLE_ACTIVE_INFO
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @returns { Promise<Array<BundleActiveState>> } the state data of all bundles.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @systemapi Hide this for inner system use.
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryBundleActiveStates(begin: number, end: number): Promise<Array<BundleActiveState>>;

  /**
   * Queries state data of the current bundle within a specified period.
   *
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @param { AsyncCallback<Array<BundleActiveState>> } callback - the state data of the current bundle.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryCurrentBundleActiveStates(
    begin: number,
    end: number,
    callback: AsyncCallback<Array<BundleActiveState>>
  ): void;

  /**
   * Queries state data of the current bundle within a specified period.
   *
   * @param begin Indicates the start time of the query period, in milliseconds.
   * @param end Indicates the end time of the query period, in milliseconds.
   * @returns { Promise<Array<BundleActiveState>> } the state data of the current bundle.
   * @syscap SystemCapability.ResourceSchedule.UsageStatistics.App
   * @since 7
   * @deprecated since 9
   * @useinstead @ohos.resourceschedule.usageStatistics
   */
  function queryCurrentBundleActiveStates(begin: number, end: number): Promise<Array<BundleActiveState>>;
}

export default bundleState;