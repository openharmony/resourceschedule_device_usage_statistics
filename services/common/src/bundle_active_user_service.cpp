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

#include "bundle_active_user_service.h"

namespace OHOS {
namespace BundleActive {
BundleActiveUserService::BundleActiveUserService(int userId)
{
    currentStats_.reserve(BundleActivePeriodStats::PERIOD_COUNT);
    userId_ = userId;
    statsChanged_ = false;
}

void BundleActiveUserService::NotifyStatsChanged()
{
    if (!statsChanged_) {
        statsChanged_ = true;
    }
}

void BundleActiveUserService::ReportEvent(BundleActiveEvent event)
{
    BundleActivePeriodStats currentDailyStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (event.eventId_ == BundleActiveEvent::ABILITY_FOREGROUND) {
        if (!event.bundleName_.empty() && event.bundleName_ != lastBackgroundBundle_) {
            incrementBundleLaunch_ = true;
        }
    } else if (event.eventId_ == BundleActiveEvent::ABILITY_BACKGROUND) {
        if (!event.bundleName_.empty()) {
            lastBackgroundBundle_ = event.bundleName_;
        }
    }
    for (int i = 0; i < currentStats_.size(); i++) {
        switch (event.eventId_) {
            case BundleActiveEvent::SCREEN_INTERACTIVE:
                currentStats_[i].UpdateScreenInteractive(event.timeStamp_);
                break;
            case BundleActiveEvent::SCREEN_NON_INTERACTIVE:
                currentStats_[i].UpdateScreenNonInteractive(event.timeStamp_);
                break;
            case BundleActiveEvent::KEYGUARD_SHOWN:
                currentStats_[i].UpdateKeyguardShown(event.timeStamp_);
                break;
            case BundleActiveEvent::KEYGUARD_HIDDEN:
                currentStats_[i].UpdateKeyguardHidden(event.timeStamp_);
                break;
            default:
                currentStats_[i].Update(event.bundleName_, event.longTimeTaskName_, event.timeStamp_,
                    event.eventId_, event.abilityId_);
                if (incrementBundleLaunch_) {
                    currentStats_[i].bundleStats_[event.bundleName_].IncrementBundleLaunchedCount();
                }
                break;
        }
    }
}
}
}
