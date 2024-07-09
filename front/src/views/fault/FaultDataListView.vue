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
    this.faultDataRefreh();
    // this.getChartData();
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
      let unit = '';
      fiveData.forEach(item => {
        if (!groupedData[item.selfNode]) {
          groupedData[item.selfNode] = {};
        }
        const key = `${item.pairNode}-${item.isClient}-${item.myPort}`;
        if (!groupedData[item.selfNode][key]) {
          groupedData[item.selfNode][key] = [];
        }
        if (this.optionsValue === 'transfer') {
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(), item.transfer]);
          unit = "transfer/(byte)";
        } else if (this.optionsValue === 'band') {
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.band]);
          unit = "band/(byte/s)";
        } else if (this.optionsValue === 'err') {
          unit = "err/(个)";
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.err]);
        } else if (this.optionsValue === 'rtry') {
          unit = "rtry/(个)";
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.rtry]);
        } else if (this.optionsValue === 'rtt') {
          unit = "rtt/(us)";
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.rtt]);
        } else if (this.optionsValue === 'lost') {
          unit = "lost/(%)";
          groupedData[item.selfNode][key].push([new Date(item.sec * 1000 / 1000).toLocaleString(),item.lost]);
        }
      });
      // 更新曲线
      this.initFaultChart(groupedData, unit);
    },
    initFaultChart(groupedData, unit) {
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
            data: innerData[subKey]
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
          data: legendData // 将对象的键作为 legend 数据
        },
        grid: {
          left: '1%',
          right: '4%',
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
          boundaryGap: false,
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
          // grid: {
          //   left:
          // },
        },
        series: seriesData
      };
      option && this.faultChart.setOption(option);
    },
  }
}
</script>


<style scoped>
#fault {
  margin: 0 auto;
}

</style>