# 设备使用信息统计<a name="ZH-CN_TOPIC_0000001115588558"></a>
-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [说明](#section1312121216216)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

设备使用信息统计，包括app usage/notification usage/system usage等使用统计。例如应用使用信息统计，用于保存和查询应用使用详情（app usage）、事件日志数据（event log）、应用分组（bundle group）情况。
部件缓存的应用记录（使用历史统计和使用事件记录）会在事件上报后30分钟内刷新到数据库持久化保存。

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
│   ├── JS                             # 对外JS接口实现目录
│   └── native                         # 对外Native接口实现目录
├── adapter                            # 适配目录
└── test                               # 测试用例目录
```

## 说明<a name="section1312121216216"></a>

### 接口说明<a name="section1551164914237"></a>

设备使用信息统计接口，包括app usage/notification usage/system usage等接口，以app usage接口为例，对外提供主要接口如下。

<a name="table775715438253"></a>
<table><thead align="left"><tr id="row12757154342519"><th class="cellrowborder" valign="top" width="73%" id="mcps1.1.3.1.1"><p id="p1075794372512"><a name="p1075794372512"></a><a name="p1075794372512"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="56.81%" id="mcps1.1.3.1.2"><p id="p375844342518"><a name="p375844342518"></a><a name="p375844342518"></a>接口描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1975804332517"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p5758174313255"><a name="p5758174313255"></a><a name="p5758174313255"></a>queryBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p14758743192519"><a name="p14758743192519"></a><a name="p14758743192519"></a>通过指定起始和结束时间查询所有应用的事件集合。</p>
</td>
</tr>
<tr id="row2758943102514"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p107581438250"><a name="p107581438250"></a><a name="p107581438250"></a>queryBundleStateInfos(begin::number, end::number, callback:AsyncCallback&lt;BundleStateInfoResponse&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p8758743202512"><a name="p8758743202512"></a><a name="p8758743202512"></a>使用起始和结束时间来查询应用使用时长统计信息。</p>
</td>
</tr>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryCurrentBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>通过时间段间隔（天、周、月、年）等查询当前应用的事件集合。</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryBundleStateInfoByInterval(byInterval:intervalType, begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleStateInfo&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>通过时间段间隔（天、周、月、年）查询应用使用时长统计信息。</p>
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

设备使用信息统计接口众多，以应用使用详情（app usage）接口为例，介绍接口逻辑。

- **运行进程**：设备使用信息统计服务在foundation进程启动和运行。
- **应用使用统计信息落盘时机**：
>1.  每隔30分钟触发一次刷新；
>2.  系统时间变更触发一次刷新；
>3.  下一天开始触发一次刷新；
- **应用查询接口**：
>1.  根据起止时间查询所有应用的事件集合；
>2.  根据起止时间查询应用的使用时长；
>3.  根据起止时间查询当前应用的事件集合；
>4.  根据interval（日、周、月、年）类型和起止时间查询应用的使用时长；
>5.  查询调用者应用的优先级群组；
>5.  判断指定应用当前是否是空闲状态；

## 相关仓<a name="section1371113476307"></a>

全局资源调度子系统

**device\_usage\_statistics**

resource_schedule_service

appexecfwk_standard

native_appdatamgr