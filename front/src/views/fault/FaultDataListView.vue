<template>
  <div>
    <el-card>
      <p style="text-align: center">故障数据显示界面</p>
      <div style="float: right">
        <span>故障类型：</span>
        <el-select v-model="optionsValue" placeholder="请选择">
          <el-option
              v-for="item in options"
              :key="item.value"
              :label="item.label"
              :value="item.value">
          </el-option>
        </el-select>
      </div>
      <div id="fault" style="width: 80%; height: 400px">
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
      faultResultData: {
        "bandWidth": "",
        "retry": "6,"
      },
      intervalId: null,
      options: [{
        value: 'bandwidth',
        label: 'bandwidth'
      }, {
        value: 'retry',
        label: 'retry'
      },],
      optionsValue: 'bandwidth',
    }
  },
  mounted() {
    this.initFaultChart(this.faultDataArry);
    this.getFaultResult();
  },
  watch: {
    optionsValue(newValue) {
      // 根据选项值，更新曲线数据
      if (newValue === 'bandwidth') {
        const bandwidthData = this.faultResultData.bandWidth.split(',');
        this.faultDataArry = bandwidthData;
        this.initFaultChart(bandwidthData);
      } else if (newValue ==='retry') {
        const retryData = this.faultResultData.retry.split(',');
        this.faultDataArry = retryData;
        this.initFaultChart(retryData);
      }
    },
    faultResultData: {
      handler(newValue) {
        // 每隔 10 秒调用 getFaultResult 方法
        this.intervalId = setInterval(() => this.getFaultResult(), 10000);
        // 更新数据
        let currentBandWidth = [];
        if (this.optionsValue === 'bandwidth') {
          currentBandWidth = this.faultResultData.bandWidth.split(',');
        } else if (this.optionsValue ==='retry') {
          currentBandWidth = this.faultResultData.retry.split(',');
        }
        this.updateFaultChart(currentBandWidth);
      },
      deep: true
    }
  },
  methods: {
    // sse() {
    //   // 检查浏览器是否支持SSE
    //   if ("EventSource" in window) {
    //     // 创建EventSource对象并指定SSE服务器的路径
    //     let source = new EventSource("http://127.0.0.1:8888/api/sse");
    //     console.log('ss:', source);
    //     // 监听消息的传输
    //     source.onmessage = function(e) {
    //       // 对接收到的数据进行处理
    //       console.log('接收消息：', e.data);
    //     };
    //
    //     // 连接建立成功时触发
    //     source.onclose = function() {
    //       console.log("连接关闭.");
    //     };
    //
    //     // 连接发生错误时触发
    //     source.onerror = function(e) {
    //       console.log("链接出错：", e);
    //     };
    //   } else {
    //     console.log("浏览器不支持SSE");
    //   }
    // },
    async getFaultResult() {
      try {
        // 发送请求
        const res = await axios.post("/api/get_fault_data", {}, {});
        if (res.data.code === 200) {
          const message = JSON.parse(res.data.message);
          this.faultResultData.bandWidth += message.bandWidth + ',';
          this.faultResultData.retry += message.retry + ',';
          console.log(this.faultResultData);
        }  else if (res.data.code === 601) {
          // 当 code 为 601 时，取消定时函数，停止循环调用
          clearInterval(this.intervalId);
        }
      } catch (error) {
        console.error('Error:', error);
      }
    },
    initFaultChart(faultDataArray) {
      if (!this.faultChart) {
        let chartDom = document.getElementById('fault');
        this.faultChart = echarts.init(chartDom);
      }
      let option;
      option = {
        title: {
          text: '故障数据图'
        },
        // 数据显示
        tooltip: {
          trigger: 'axis'
        },
        // 图示例
        // legend: {
        //   data: ['Email', 'Union Ads', 'Video Ads', 'Direct', 'Search Engine']
        // },
        grid: {
          left: '1%',
          right: '4%',
          bottom: '3%',
          containLabel: true
        },
        // 图表下载按钮
        // toolbox: {
        //   feature: {
        //     saveAsImage: {}
        //   }
        // },
        // 横坐标
        xAxis: {
          type: 'category',
          boundaryGap: false,
          data: Array.from({ length: faultDataArray.length }, (_, i) => i + 1)
        },
        // 纵坐标
        yAxis: {
          type: 'value'
        },
        series: [
          {
            name: 'Data',
            type: 'line',
            stack: 'Total',
            data: faultDataArray
          },
        ]
      };
      option && this.faultChart.setOption(option);
    },
    updateFaultChart(currentBandWidth) {
      // 更新曲线数据
      this.faultChart.setOption({
        xAxis: {
          data: Array.from({ length: currentBandWidth.length }, (_, i) => i + 1)
        },
        series: [
          {
            data: currentBandWidth,
          },
        ],
      });
    },
  }
}
</script>


<style scoped>
#fault {
  margin: 0 auto;
}

</style>