# ohos-usageStatsQuery 测试用例

## 测试覆盖说明

本文档覆盖所有10个CLI命令的所有参数组合示例，确保100%测试覆盖率。

| 命令示例 | 说明 | 权限 | 前置依赖 |
|------|------|------|----------|
| `ohos-usageStatsQuery check-bundle-idle --bundle com.example.app` | 检查应用是否为常用应用（必需参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery check-bundle-idle --bundle com.example.app --user 100` | 检查应用是否为常用应用（所有参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery check-bundle-idle --bundle com.example.app --user -1` | 检查应用是否为常用应用（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery check-bundle-period --bundle com.example.app` | 检查应用使用时段（必需参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery check-bundle-period --bundle com.example.app --user 100` | 检查应用使用时段（所有参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery check-bundle-period --bundle com.example.app --user -1` | 检查应用使用时段（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 0 --begin 1609459200000 --end 1609545600000` | 查询使用统计（按天） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 1 --begin 1609459200000 --end 1610064000000` | 查询使用统计（按周） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 2 --begin 1609459200000 --end 1612137600000` | 查询使用统计（按月） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 3 --begin 1609459200000 --end 1640995200000` | 查询使用统计（按年） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 1 --begin 1609459200000 --end 1609545600000 --user 100` | 查询使用统计（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-stats-interval --interval 1 --begin 1609459200000 --end 1609545600000 --user -1` | 查询使用统计（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000` | 查询事件记录（必需参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --max 1000` | 查询事件记录（默认最大数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --max 500` | 查询事件记录（自定义数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --max 1` | 查询事件记录（最小数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --user 100` | 查询事件记录（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --user -1` | 查询事件记录（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-events --begin 1609459200000 --end 1609545600000 --user 100 --max 500` | 查询事件记录（所有参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-app-group --bundle com.example.app` | 查询应用分组（指定应用） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-app-group --bundle com.example.app --user 100` | 查询应用分组（指定用户和应用） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-app-group --bundle com.example.app --user -1` | 查询应用分组（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle` | 查询高频应用（默认参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --user -1` | 查询高频应用（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --user 100` | 查询高频应用（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --max 20` | 查询高频应用（默认数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --max 50` | 查询高频应用（自定义数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --max 1` | 查询高频应用（最小数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --days 7` | 查询高频应用（默认天数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --days 14` | 查询高频应用（自定义天数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --days 1` | 查询高频应用（最小天数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-bundle --user 100 --max 50 --days 14` | 查询高频应用（所有参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records` | 查询模块记录（默认参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --max 1000` | 查询模块记录（默认数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --max 500` | 查询模块记录（自定义数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --max 1` | 查询模块记录（最小数量） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --user -1` | 查询模块记录（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --user 100` | 查询模块记录（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-module-records --max 500 --user 100` | 查询模块记录（所有参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-notification-stats --begin 1609459200000 --end 1609545600000` | 查询通知统计（必需参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-notification-stats --begin 1609459200000 --end 1609545600000 --user 100` | 查询通知统计（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-notification-stats --begin 1609459200000 --end 1609545600000 --user -1` | 查询通知统计（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-period` | 查询高频时段（默认参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-period --user -1` | 查询高频时段（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-high-freq-period --user 100` | 查询高频时段（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-latest-used-time --bundle com.example.app` | 查询今日使用时间（必需参数） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-latest-used-time --bundle com.example.app --user 100` | 查询今日使用时间（指定用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery query-latest-used-time --bundle com.example.app --user -1` | 查询今日使用时间（默认用户） | ohos.permission.cli.BUNDLE_ACTIVE_INFO | 无 |
| `ohos-usageStatsQuery --help` | 查看帮助信息 | 无 | 无 |
| `ohos-usageStatsQuery help` | 查看帮助信息 | 无 | 无 |
| `ohos-usageStatsQuery help check-bundle-idle` | 查看命令帮助（check-bundle-idle） | 无 | 无 |
| `ohos-usageStatsQuery help check-bundle-period` | 查看命令帮助（check-bundle-period） | 无 | 无 |
| `ohos-usageStatsQuery help query-stats-interval` | 查看命令帮助（query-stats-interval） | 无 | 无 |
| `ohos-usageStatsQuery help query-events` | 查看命令帮助（query-events） | 无 | 无 |
| `ohos-usageStatsQuery help query-app-group` | 查看命令帮助（query-app-group） | 无 | 无 |
| `ohos-usageStatsQuery help query-high-freq-bundle` | 查看命令帮助（query-high-freq-bundle） | 无 | 无 |
| `ohos-usageStatsQuery help query-module-records` | 查看命令帮助（query-module-records） | 无 | 无 |
| `ohos-usageStatsQuery help query-notification-stats` | 查看命令帮助（query-notification-stats） | 无 | 无 |
| `ohos-usageStatsQuery help query-high-freq-period` | 查看命令帮助（query-high-freq-period） | 无 | 无 |
| `ohos-usageStatsQuery help query-latest-used-time` | 查看命令帮助（query-latest-used-time） | 无 | 无 |

## 测试覆盖率统计

- **总命令数**: 10个
- **总测试用例数**: 58个
- **覆盖率**: 100%

## 参数组合统计

| 命令 | 必需参数 | 可选参数 | 参数组合数 |
|------|---------|---------|-----------|
| check-bundle-idle | bundle | user | 3 |
| check-bundle-period | bundle | user | 3 |
| query-stats-interval | interval, begin, end | user | 7 |
| query-events | begin, end | user, max | 7 |
| query-app-group | bundle | user | 3 |
| query-high-freq-bundle | 无 | user, max, days | 8 |
| query-module-records | 无 | max, user | 7 |
| query-notification-stats | begin, end | user | 3 |
| query-high-freq-period | 无 | user | 3 |
| query-latest-used-time | bundle | user | 3 |

## 边界测试

### 时间参数边界
- begin和end必须为正整数（单位：毫秒）
- end必须大于begin

### 数量参数边界
- max范围：1-1000
- days范围：1-30（建议1-14）

### 用户ID边界
- user范围：-1（默认）或正整数

### intervalType边界
- interval范围：0-3（0=按天，1=按周，2=按月，3=按年）

## 权限说明

除help命令外，所有命令均需要：
- **ohos.permission.cli.BUNDLE_ACTIVE_INFO**: 应用使用统计信息查询权限

## 前置依赖说明

所有命令均无前置依赖，可独立执行。