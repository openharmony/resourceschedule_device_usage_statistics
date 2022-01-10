# 设备使用信息统计<a name="ZH-CN_TOPIC_0000001115588558"></a>
-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [说明](#section1312121216216)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

设备使用信息统计用于保存和查询应用使用详情、事件日志数据、应用分组情况。部件缓存的应用记录（使用历史统计和使用事件记录）会定期刷新到数据库进行持久化保存。

## 目录<a name="section161941989596"></a>

```
/foundation/resourceschedule/device_usage_statistics
├── ohos.build                         # 编译脚本
├── BUILD.gn                           # 模块编译脚本
├── interfaces
│   ├── innerkits                      # 对内接口目录
│   └── kits                           # 对外接口目录
├── services                           # 服务层目录
├── frameworks
│   ├── JS                             # 外部JS接口实现目录
│   └── native                         # 外部native接口实现目录
├── adapter                            # 适配目录
└── test                               # 测试用例目录
```

## 说明<a name="section1312121216216"></a>

### 接口说明<a name="section1312121216216"></a>

#### 内部接口说明<a name="section1551164914237"></a>

<a name="table775715438253"></a>
<table><thead align="left"><tr id="row12757154342519"><th class="cellrowborder" valign="top" width="73%" id="mcps1.1.3.1.1"><p id="p1075794372512"><a name="p1075794372512"></a><a name="p1075794372512"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="56.81%" id="mcps1.1.3.1.2"><p id="p375844342518"><a name="p375844342518"></a><a name="p375844342518"></a>接口描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1975804332517"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p5758174313255"><a name="p5758174313255"></a><a name="p5758174313255"></a>int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId);</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p14758743192519"><a name="p14758743192519"></a><a name="p14758743192519"></a>采集数据上报接口。</p>
</td>
</tr>
<tr id="row2758943102514"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p107581438250"><a name="p107581438250"></a><a name="p107581438250"></a>int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId);</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p8758743202512"><a name="p8758743202512"></a><a name="p8758743202512"></a>判断应用是否处于不活跃状态。</p>
</td>
</tr>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>std::vector<BundleActiveUsageStats> QueryUsageStats(int userId, int intervalType, int64_t beginTime, int64_t endTime);</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>查询指定用户应用使用时长信息。</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>std::vector<BundleActiveEvent> QueryEvents(int userId, int64_t beginTime, int64_t endTime);</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>查询指定用户使用事件信息。</p>
</td>
</tr>
</tbody>
</table>

#### 外部接口说明<a name="section1551164914237"></a>

<a name="table775715438253"></a>
<table><thead align="left"><tr id="row12757154342519"><th class="cellrowborder" valign="top" width="73%" id="mcps1.1.3.1.1"><p id="p1075794372512"><a name="p1075794372512"></a><a name="p1075794372512"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="56.81%" id="mcps1.1.3.1.2"><p id="p375844342518"><a name="p375844342518"></a><a name="p375844342518"></a>接口描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1975804332517"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p5758174313255"><a name="p5758174313255"></a><a name="p5758174313255"></a>queryBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p14758743192519"><a name="p14758743192519"></a><a name="p14758743192519"></a>通过时间段间隔查询所有应用的事件集合。</p>
</td>
</tr>
<tr id="row2758943102514"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p107581438250"><a name="p107581438250"></a><a name="p107581438250"></a>queryBundleStateInfos(begin::number, end::number, callback:AsyncCallback&lt;BundleStateInfoResponse&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p8758743202512"><a name="p8758743202512"></a><a name="p8758743202512"></a>使用起始和结束时间来查询应用使用时长统计信息。</p>
</td>
</tr>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryCurrentBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>通过时间段间隔查询当前应用的事件集合。</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryBundleStateInfoByInterval(byInterval:intervalType, begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleStateInfo&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>通过时间段间隔查询应用使用时长统计信息。</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryAppUsagePriorityGroup(callback:AsyncCallback&lt;number&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>查询（返回）当前调用者应用的使用优先级群组。</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>isIdleState(bundleName:string, callback:AsyncCallback&lt;boolean&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>判断指定Bundle Name的应用当前是否是空闲状态。</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

- **运行进程**：设备使用信息统计服务在foundation进程启动和运行。
- **上报事件接口**：
>1.  应用程序框架子系统上报Ability生命周期事件到设备使用信息统计部件，部件开启一个独立线程异步处理，防止同步处理阻塞应用程序框架子系统运行逻辑；
>2.  电源管理子系统上报系统关机事件到设备使用信息统计部件，部件做同步处理；
- **应用使用统计信息落盘时机**：
>1.  每隔30分钟触发一次刷新；
>2.  系统时间变更触发一次刷新；
>3.  下一天开始触发一次刷新；

## 相关仓<a name="section1371113476307"></a>

全局资源调度子系统

**device\_usage\_statistics**

resource_schedule_service

appexecfwk_standard

native_appdatamgr