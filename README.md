# Device usage statistics<a name="EN-US_TOPIC_0000001115588558"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Instruction](#section1312121216216)
    -   [Available APIs](#section1551164914237)
    -   [Usage Guidelines](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

The **device usage statistics** includes app usage, notification usage, system usage and other usage statistics. For example, application usage statistics is used to save 
and query application usage details, event log data and application grouping.The application records (usage history statistics and usage event records) cached by 
the component will be periodically refreshed to the database for persistent storage.

## Directory Structure<a name="section161941989596"></a>

```
/foundation/resourceschedule/device_usage_statistics
├── ohos.build                         # Compilation script
├── BUILD.gn                           # Module compilation script
├── interfaces
│   ├── innerkits                      # Internal interface directory
│   └── kits                           # External interface directory
├── services                           # Service layer directory
├── frameworks
│   ├── JS                             # External JS interface implementation directory
│   └── native                         # External native interface implementation directory
├── adapter                            # Adaptation directory
└── test                               # Testing case directory
```

## Instruction<a name="section1312121216216"></a>

### Available APIs<a name="section1551164914237"></a>

Device usage statistics interfaces include app usage, notification usage, system usage and other interfaces. 
Taking app usage interface as an example, the main exposed interfaces are as follows.

<a name="table775715438253"></a>
<table><thead align="left"><tr id="row12757154342519"><th class="cellrowborder" valign="top" width="60%" id="mcps1.1.3.1.1"><p id="p1075794372512"><a name="p1075794372512"></a><a name="p1075794372512"></a>API name</p>
</th>
<th class="cellrowborder" valign="top" width="56.81%" id="mcps1.1.3.1.2"><p id="p375844342518"><a name="p375844342518"></a><a name="p375844342518"></a>API description</p>
</th>
</tr>
</thead>
<tbody><tr id="row1975804332517"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p5758174313255"><a name="p5758174313255"></a><a name="p5758174313255"></a>queryBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p14758743192519"><a name="p14758743192519"></a><a name="p14758743192519"></a>Queries the event collection of all applications through time interval.</p>
</td>
</tr>
<tr id="row2758943102514"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p107581438250"><a name="p107581438250"></a><a name="p107581438250"></a>queryBundleStateInfos(begin::number, end::number, callback:AsyncCallback&lt;BundleStateInfoResponse&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p8758743202512"><a name="p8758743202512"></a><a name="p8758743202512"></a>Uses the start and end time to query the application usage time statistics.</p>
</td>
</tr>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryCurrentBundleActiveStates(begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleActiveState&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>Queries the event collection of the current application through the time interval.</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryBundleStateInfoByInterval(byInterval:intervalType, begin::number, end::number, callback:AsyncCallback&lt;Array&lt;BundleStateInfo&gt;&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>Queries application usage duration statistics by time interval.</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>queryAppUsagePriorityGroup(callback:AsyncCallback&lt;number&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>Queries (returns) the priority group used by the current caller application.</p>
<tr id="row09311240175710"><td class="cellrowborder" valign="top" width="43.19%" headers="mcps1.1.3.1.1 "><p id="p159328405571"><a name="p159328405571"></a><a name="p159328405571"></a>isIdleState(bundleName:string, callback:AsyncCallback&lt;boolean&gt;):void</p>
</td>
<td class="cellrowborder" valign="top" width="56.81%" headers="mcps1.1.3.1.2 "><p id="p493294018574"><a name="p493294018574"></a><a name="p493294018574"></a>Judges whether the application of the specified bundle name is currently idle.</p>
</td>
</tr>
</tbody>
</table>

### Usage Guidelines<a name="section129654513264"></a>

There are many interfaces for device usage statistics. Take app usage interface as an example to introduce the interface logic.

- **running process**:The device usage statistics service starts and runs in the foundation process.
- **device usage statistics saving time**:
>1.  refreshing is triggered every 30 minutes;
>2.  refreshing is triggered when system time changes;
>3.  refreshing is triggered from the next day;
- **app querying interface**:
>1.  Query the event collection of all applications according to the start and end time;
>2.  Query the usage duration of the application according to the start and end time;
>3.  Query the event collection of the current application according to the start and end time;
>4.  Query the usage duration of the application according to the type of interval (day, week, month, year) and the start and end time;
>5.  Query the priority group of the caller application;
>6.  Judge whether the specified application is currently idle;

## Repositories Involved<a name="section1371113476307"></a>

resource schedule subsystem

**device\_usage\_statistics**

resource_schedule_service

appexecfwk_standard

native_appdatamgr