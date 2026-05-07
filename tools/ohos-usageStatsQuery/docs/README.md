# ohos-usageStatsQuery

## 概述

ohos-usageStatsQuery 是一个应用使用统计查询工具，用于查询应用的使用情况、事件记录、高频使用时段等信息。

## 功能列表

- 查询应用空闲状态
- 查询应用使用周期状态
- 查询时间段内的使用统计信息
- 查询应用事件记录
- 查询应用分组信息
- 查询高频使用的应用列表
- 查询模块使用记录
- 查询通知事件统计
- 查询高频使用时段
- 查询应用今日最新使用时间

## 依赖

- 系统能力：BundleActiveService (SA ID: 1907)
- 权限：ohos.permission.BUNDLE_ACTIVE_INFO

## 基本用法

```bash
ohos-usageStatsQuery <command> [options]
```

## 命令列表

| 命令 | 说明 | 参数 | 权限 | 前置依赖 |
|------|------|------|------|----------|
| isBundleIdle | 查询应用空闲状态 | --bundleName <string>, [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| isBundleUsePeriod | 查询应用使用周期状态 | --bundleName <string>, [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryBundleStatsInfoByInterval | 查询时间段内使用统计 | --intervalType <int>, --beginTime <long>, --endTime <long>, [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryBundleEvents | 查询时间段内事件记录 | --beginTime <long>, --endTime <long>, [--userId <int>], [--maxNum <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryAppGroup | 查询应用分组信息 | [--bundleName <string>], [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryHighFrequencyUsageBundleInfos | 查询高频使用应用列表 | [--userId <int>], [--maxNum <int>], [--queryDayRange <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryModuleUsageRecords | 查询模块使用记录 | [--maxNum <int>], [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryNotificationEventStats | 查询通知事件统计 | --beginTime <long>, --endTime <long>, [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryHighFrequencyPeriodBundle | 查询高频使用时段 | [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |
| queryBundleTodayLatestUsedTime | 查询今日最新使用时间 | --bundleName <string>, [--userId <int>] | ohos.permission.BUNDLE_ACTIVE_INFO | 无 |

**前置依赖说明**：
- **无**：该命令可直接执行，无需前置条件

## 参数说明

### 时间参数
- `beginTime`: 开始时间，毫秒级时间戳（例如：1609459200000）
- `endTime`: 结束时间，毫秒级时间戳（例如：1609545600000）
- `queryDayRange`: 查询天数范围（默认：7天）

### intervalType参数
- 0: BY_OPTIMIZED（优化查询）
- 1: BY_DAILY（按天）
- 2: BY_WEEKLY（按周）
- 3: BY_MONTHLY（按月）
- 4: BY_ANNUALLY（按年）

### userId参数
- -1: 当前用户（默认值）
- 其他值：指定用户ID

### maxNum参数
- 范围：1-1000
- 默认值因命令不同而异

## 示例

### 1. 查询应用空闲状态

```bash
# 查询指定应用是否空闲
ohos-usageStatsQuery isBundleIdle --bundleName com.example.app

# 查询指定用户的应用空闲状态
ohos-usageStatsQuery isBundleIdle --bundleName com.example.app --userId 100
```

### 2. 查询应用使用周期状态

```bash
# 查询指定应用是否在使用周期
ohos-usageStatsQuery isBundleUsePeriod --bundleName com.example.app

# 查询指定用户的应用使用周期
ohos-usageStatsQuery isBundleUsePeriod --bundleName com.example.app --userId 100
```

### 3. 查询时间段内使用统计

```bash
# 按天查询使用统计
ohos-usageStatsQuery queryBundleStatsInfoByInterval --intervalType 1 --beginTime 1609459200000 --endTime 1609545600000

# 按周查询指定用户的使用统计
ohos-usageStatsQuery queryBundleStatsInfoByInterval --intervalType 2 --beginTime 1609459200000 --endTime 1610064000000 --userId 100
```

### 4. 查询时间段内事件记录

```bash
# 查询事件记录
ohos-usageStatsQuery queryBundleEvents --beginTime 1609459200000 --endTime 1609545600000

# 查询事件记录并限制数量
ohos-usageStatsQuery queryBundleEvents --beginTime 1609459200000 --endTime 1609545600000 --maxNum 500 --userId 100
```

### 5. 查询应用分组信息

```bash
# 查询指定应用的分组信息
ohos-usageStatsQuery queryAppGroup --bundleName com.example.app

# 查询当前应用的分组信息（bundleName为空）
ohos-usageStatsQuery queryAppGroup

# 查询指定用户的应用分组
ohos-usageStatsQuery queryAppGroup --bundleName com.example.app --userId 100
```

### 6. 查询高频使用应用列表

```bash
# 查询高频使用应用（默认参数）
ohos-usageStatsQuery queryHighFrequencyUsageBundleInfos

# 查询前50个高频应用
ohos-usageStatsQuery queryHighFrequencyUsageBundleInfos --maxNum 50 --queryDayRange 14

# 查询指定用户的高频应用
ohos-usageStatsQuery queryHighFrequencyUsageBundleInfos --userId 100 --maxNum 50 --queryDayRange 14
```

### 7. 查询模块使用记录

```bash
# 查询模块使用记录
ohos-usageStatsQuery queryModuleUsageRecords

# 查询指定数量的模块记录
ohos-usageStatsQuery queryModuleUsageRecords --maxNum 500

# 查询指定用户的模块记录
ohos-usageStatsQuery queryModuleUsageRecords --maxNum 500 --userId 100
```

### 8. 查询通知事件统计

```bash
# 查询通知事件统计
ohos-usageStatsQuery queryNotificationEventStats --beginTime 1609459200000 --endTime 1609545600000

# 查询指定用户的通知事件统计
ohos-usageStatsQuery queryNotificationEventStats --beginTime 1609459200000 --endTime 1609545600000 --userId 100
```

### 9. 查询高频使用时段

```bash
# 查询高频使用时段
ohos-usageStatsQuery queryHighFrequencyPeriodBundle

# 查询指定用户的高频使用时段
ohos-usageStatsQuery queryHighFrequencyPeriodBundle --userId 100
```

### 10. 查询今日最新使用时间

```bash
# 查询指定应用今日最新使用时间
ohos-usageStatsQuery queryBundleTodayLatestUsedTime --bundleName com.example.app

# 查询指定用户的应用使用时间
ohos-usageStatsQuery queryBundleTodayLatestUsedTime --bundleName com.example.app --userId 100
```

## 输出格式

所有命令输出JSON格式，包含以下字段：

```json
{
  "type": "result",
  "status": "success | failed",
  "data": { ... },
  "errCode": "ERROR_CODE",
  "errMsg": "Error description",
  "suggestion": "Suggested solution"
}
```

## 权限说明

所有命令都需要以下权限：
- **ohos.permission.BUNDLE_ACTIVE_INFO**: 应用使用统计信息查询权限

## 注意事项

1. 所有时间参数使用毫秒级时间戳
2. userId默认值为-1，表示当前用户
3. IsBundleUsePeriod命令仅支持Native进程调用
4. QueryAppGroup查询自己应用时，bundleName可以为空

## 使用场景

- **系统运维**: 查看应用使用情况，分析系统性能
- **性能分析**: 分析高频使用应用，优化资源分配
- **问题排查**: 查询应用事件记录，定位应用行为问题
- **用户行为分析**: 分析应用使用时段和频率