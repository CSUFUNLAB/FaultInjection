const DataStreamColumn = [
    { label: '起始节点号', prop: 'nodeA', width: '200', key: '100' },
    { label: '目标节点', prop: 'nodeB', width: '200', key: '200' ,},
    { label: '类型(tcp/udp)', prop: 'type', width: '200', key: '300', },
    { label: '带宽(bps)', prop: 'bandWidth', width: '200', key: '400',  },
    { label: '持续时间(s)', prop: 'sendTime', width: '200', key: '500', },
];

const DataStreamInputData = [
    {nodeA: '', nodeB: '', type: '', bandWidth: '', sendTime: ''},
]
const DataStreamOutputData = [
    {nodeA: '11', nodeB: '22', type: '33', bandWidth: '44', sendTime: '2'},
]
export {
    DataStreamColumn, DataStreamInputData, DataStreamOutputData
}