<template>
  <div>
    <el-card>
      <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">故障数据显示界面</p>
      <div style="float: right; ">
        <span>数据类型：</span>
        <el-select v-model="optionsValue" placeholder="请选择" @change="changeDataType">
          <el-option
              v-for="item in options"
              :key="item.value"
              :label="item.label"
              :value="item.value">
          </el-option>
        </el-select>
      </div>
      <div id="fault" style="width: 100%; height: 500px; margin-top: 60px;">
      </div>
    </el-card>
  </div>
</template>

<script>
import * as echarts from 'echarts';
import axios from "axios";
export default {
  name: "FaultDataListView",
  data() {
    return {
      globalOption: '',
      faultChart: '',
      faultDataArry: [],
      faultResultData: [],
      intervalId: null,
      options: [{
        value: 'transfer',
      },{
        value: 'band',
      },{
        value: 'err',
      },{
        value: 'rtry',
      },{
        value: 'rtt',
      },{
        value: 'lost',
      },],
      optionsValue: 'transfer',
    }
  },
  created() {
    // this.faultDataRefreh();
    this.getChartData();
  },
  destroyed() {
    // 页面销毁后，清除计时器
    this.clear();
  },
  methods: {
    // 获取数据定时器
    faultDataRefreh() {
      if (this.intervalId != null) {
        return;
      }
      // 计时器为空，获取数据
      this.intervalId = setInterval(() => {
        this.getChartData();
      }, 1000);
    },
    // 停止定时器
    clear() {
      clearInterval(this.intervalId); // 清楚计时器
      this.intervalId = null;
    },
    // 数据类型改变
    changeDataType(value) {
      this.getChartData();
    },
    // 获取数据
    async getChartData() {
      try {
        const response = await axios.get('/front/get_fault_result');

        const dataRows = response.data.data.split('\n').filter(row => row!== '');
        const fiveData = [];

        dataRows.forEach(row => {
          const columns = row.split(',');
          fiveData.push({
            selfNode: columns[0],
            pairNode: columns[1],
            isClient: columns[2],
            myPort: columns[3],
            transType: columns[4],
            sec: columns[5],
            err: columns[6],
            band: columns[7],
            transfer: columns[8],
            rtry: columns[9],
            rtt: columns[10],
            lost: columns[11],
          })
        });
        this.updateFaultChart(fiveData);
      } catch (error) {
        console.error(error);
      }
    },
    // 更新表格
    updateFaultChart(fiveData) {
      // 在这里根据第一列的值进行分组和进一步处理
      const groupedData = {};
      const tipsData = {};
      let unit = '';
      fiveData.forEach(item => {
        if (!groupedData[item.selfNode]) {
          groupedData[item.selfNode] = {};
          tipsData[item.selfNode] = {};
        }
        const key = `${item.pairNode}-${item.isClient}-${item.myPort}`;
        if (!groupedData[item.selfNode][key]) {
          groupedData[item.selfNode][key] = [];
          tipsData[item.selfNode][key] = [];
        }
        if (this.optionsValue === 'transfer') {
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(), item.transfer]);
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          unit = "transfer/(byte)";
        } else if (this.optionsValue === 'band') {
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.band]);
          unit = "band/(byte/s)";
        } else if (this.optionsValue === 'err') {
          unit = "err/(个)";
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.err]);
        } else if (this.optionsValue === 'rtry') {
          unit = "rtry/(个)";
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.rtry]);
        } else if (this.optionsValue === 'rtt') {
          unit = "rtt/(us)";
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.rtt]);
        } else if (this.optionsValue === 'lost') {
          unit = "lost/(%)";
          tipsData[item.selfNode][key].push([item.isClient, item.pairNode, item.transType]);
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.lost]);
        }
      });
      // 更新曲线
      this.initFaultChart(groupedData, unit, tipsData);
    },
    initFaultChart(groupedData, unit, tipsData) {
      if (!this.faultChart) {
        let chartDom = document.getElementById('fault');
        this.faultChart = echarts.init(chartDom);
      }
      let legendData = Object.keys(groupedData);
      let seriesData = [];

      for (let key in groupedData) {
        let innerData = groupedData[key];
        for (let subKey in innerData) {
          seriesData.push({
            name: key,
            type:'line',
            smooth: true,
            data: innerData[subKey],
          });
        }
      }
      let option;
      option = {
        // 数据显示
        tooltip: {
          trigger: 'axis'
        },
        // 图示例
        legend: {
          selectedMode: 'single',
          data: legendData // 将对象的键作为 legend 数据
        },
        grid: {
          left: '8%',
          right: '8%',
          bottom: '3%',
          containLabel: true
        },
        // 横坐标
        xAxis: {
          name: '时间',
          type: 'time',
          axisLine: {
            show: true,
            symbol: ['none', 'arrow']
          },
          max: function (value) {
            if ((new Date(value.max).getTime() - new Date(value.min).getTime()) <= 120000) {
              return new Date(value.min + 120000)
            } else {
              return value.max;
            }
          },
          min: function (value) {
            if ((new Date(value.max).getTime() - new Date(value.min).getTime()) <= 120000) {
              return value.min;
            } else {
              return new Date(value.max - 2*60*1000);
            }
          }
        },
        // 纵坐标
        yAxis:{
          name: unit,
          nameRotate: '0.1',
          type: 'value',
          nameLocation: "end",
          axisLine: {
            show: true,
            symbol: ['none', 'arrow']
          },
        },
        series: seriesData
      };
      this.globalOption = option;
      option && this.faultChart.setOption(option);
      this.checkIntersection(groupedData, tipsData);
    },

    checkIntersection(groupedData, tipsData) {
      let temp = this.faultChart.getModel().getComponent('xAxis').axis.scale._extent;
      let xMin = temp[0];
      let xMax = temp[1];
      for (let key in groupedData) {
        let innerData = groupedData[key];
        for (let subKey in innerData) {
          let lineData = innerData[subKey];

          let minDiffAbove = Infinity;
          let minDiffBelow = Infinity;
          let aboveData = null;
          let belowData = null;

          for (let item of lineData) {
            let timeInMillis = new Date(item[0]).getTime();
            let diff = timeInMillis - xMin;
            if (diff === 0) {
              belowData = item;
              break;
            }
            if (diff > 0 && diff < minDiffAbove) {
              minDiffAbove = diff;
              aboveData = item;
            } else if (diff < 0 && -diff < minDiffBelow) {
              minDiffBelow = -diff;
              belowData = item;
            }
          }
          const belowTime = new Date(belowData[0]).getTime();
          let yValue = '';
          if (belowTime === xMin) {
            yValue = belowData[1];
          } else {
            const aboveTime = new Date(aboveData[0]).getTime();
            const range = aboveTime - belowTime;
            const ratio = (xMin - belowTime) / range;
            yValue = (parseFloat(belowData[1]) + (parseFloat(aboveData[1]) - parseFloat(belowData[1])) * ratio);
          }

          let tip = '';
          if (tipsData[key][subKey][0][0] === '1') {
            tip += '->'
          } else {
            tip += '<-'
          }
          if (tipsData[key][subKey][0][2] === '0') {
            tip = tip + tipsData[key][subKey][0][1] + ':tcp';
          } else {
            tip = tip + tipsData[key][subKey][0][1] + ':udp';
          }

          this.globalOption.series.push({
            name: key,
            type:'line',
            smooth: true,
            data: innerData[subKey],
            endLabel: {
              show: true,
              fontSize: 15,
              formatter: tip,
            },
            markPoint: {
              data: [
                {
                  x: '6%',
                  yAxis: yValue,
                  symbol: "pin",
                  symbolRotate: 90,
                  symbolSize: 50,
                  value: tip,
                  animation: true,
                  label: {
                    fontSize: 15,
                    show: true,
                    color: '#000'
                  },

                  itemStyle: { color: '#ffffff' }
                }],
            }
          });
        }
      }
      this.faultChart.setOption(this.globalOption);
    },
  }
}
</script>


<style scoped>
#fault {
  margin: 0 auto;
}

</style>