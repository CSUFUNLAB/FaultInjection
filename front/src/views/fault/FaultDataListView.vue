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
      <div id="fault" style="width: 100%; height: 400px; margin-top: 60px">
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
  mounted() {
    this.getChartData(this.optionsValue);
    this.initFaultChart(this.faultDataArry);
  },
  methods: {
    changeDataType(value) {
      console.log(value);
      this.getChartData(value);
    },
    async getChartData(selectedValue) {
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
        // 在这里根据第一列的值进行分组和进一步处理
        const groupedData = {};
        fiveData.forEach(item => {
          if (!groupedData[item.selfNode]) {
            groupedData[item.selfNode] = {};
          }
          const key = `${item.pairNode}-${item.isClient}-${item.myPort}`;
          if (!groupedData[item.selfNode][key]) {
            groupedData[item.selfNode][key] = [];
          }
          if (selectedValue === 'transfer') {
            groupedData[item.selfNode][key].push(item.transfer);
          } else if (selectedValue === 'band') {
            groupedData[item.selfNode][key].push(item.band);
          } else if (selectedValue === 'err') {
            groupedData[item.selfNode][key].push(item.err);
          } else if (selectedValue === 'rtry') {
            groupedData[item.selfNode][key].push(item.rtry);
          } else if (selectedValue === 'rtt') {
            groupedData[item.selfNode][key].push(item.rtt);
          } else if (selectedValue === 'lost') {
            groupedData[item.selfNode][key].push(item.lost);
          }
        });
        // 在这里可以对分组后的数据进行进一步分析处理
        console.log(groupedData);
        this.initFaultChart(groupedData);
      } catch (error) {
        console.error(error);
      }
    },
    initFaultChart(groupedData) {
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
      console.log(seriesData);

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
          type: 'category',
          boundaryGap: false,
          data: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
        },
        // 纵坐标
        yAxis: {
          type: 'value'
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