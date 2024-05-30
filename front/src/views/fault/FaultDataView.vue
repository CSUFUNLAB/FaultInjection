<template>
  <div>
    <div>
      <el-card>
        <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">节点信息</p>
        <el-button type="primary" @click="detectNodes()" class="button_class">检测节点</el-button>
        <el-button type="success" @click="functionalTest()" class="button_class" style="margin-right: 10px">功能测试</el-button>
        <el-table :data="nodeInfoData" max-height="250" border stripe highlight-current-row>
          <el-table-column
              v-for="(item, index) in nodeInfoColumn"
              :key="index"
              :prop="item.prop"
              :label="item.label"
              style="width: 30%">
          </el-table-column>
        </el-table>
      </el-card>
    </div>
    <div style="margin-top: 10px">
      <el-card>
        <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">数据流注入</p>
        <el-button type="primary" @click="insertData()" class="button_class">注入数据</el-button>
        <el-button type="info" @click="insertDataFromFile()" class="button_class" style="margin-right: 10px">导入数据</el-button>
        <el-button type="warning" @click="randomInsertData()" class="button_class">随机注入</el-button>
        <el-button type="success" @click="scanNodes()" class="button_class">扫描</el-button>
        <div>
          <div>
            <el-table :data="dataStreamInputData"
                      max-height="250"
                      stripe highlight-current-row>
              <el-table-column
                  v-for="(item, index) in dataStreamColumn"
                  :key="index"
                  :prop="item.prop"
                  :label="item.label"
                  style="width: 30%">
                <template v-slot="scope">
                  <el-input
                      v-if="true"
                      size="small"
                      ref="tableInput"
                      v-model="scope.row[item.prop]"
                      :type='getInputType(item.prop)'
                  ></el-input>
                </template>
              </el-table-column>
            </el-table>
          </div>
          <div class="other-row-border" :key="forceRefresh">
            <el-table :show-header="false"
                      :data="dataStreamOutputData"
                      style="min-height:150px"
                      max-height="250"
                      stripe highlight-current-row>
              <el-table-column
                  v-for="(item, index) in dataStreamColumn"
                  :key="index"
                  :prop="item.prop"
                  :label="item.label"
                  style="width: 30%">
                <template v-slot="scope" v-if="item.prop === 'sendTime'">
                  <el-statistic
                      @finish="hilarity(scope.$index)"
                      format="ss秒"
                      :value="scope.row[item.prop]"
                      time-indices>
                  </el-statistic>
                  <span v-if="item.prop !== 'sendTime'">{{ scope.row[item.prop] }}</span>
                </template>
              </el-table-column>
            </el-table>
          </div>
        </div>
      </el-card>
    </div>
  </div>
</template>

<script>

import axios from "axios";
import {NodeInfoColumn} from "../../../data/NodeInfoColumn";
import {DataStreamColumn, DataStreamInputData, DataStreamOutputData} from "../../../data/DataStreamColumn";

export default {
  name: "FaultDataView",
  data() {
    return {
      nodeInfoData: [],
      nodeInfoColumn: NodeInfoColumn,
      dataStreamInputData: DataStreamInputData,
      dataStreamOutputData:[],
      dataStreamColumn: DataStreamColumn,
      intervalId: null,
      processedData: new Set(), // 用于记录已处理的数据
      forceRefresh :'',
    }
  },
  mounted() {
    // 在页面加载后从本地存储中恢复数据
    this.dataStreamRefreh();
    this.checkAndRemoveExpiredData();
    // this.getDataStreamForServer();
  },
  destroyed() {
    // 页面销毁后，清除计时器
    this.clear();
  },
  methods: {
    // 功能测试按钮触发函数
    async functionalTest() {
      try {
        const res = await axios.post('/api/functional_test');
      } catch (e) {
        console.log("功能测试函数出错：", e);
      }
    },
    // 获取后端发送的输出表单数据
    async getDataStreamForServer() {
      const response = await axios.get('/front/get_data_stream');
      const data = response.data.data;

      // 获取后端发送的数据
      const newData = data.map(item => {
        const parts = item.split(',');
        return {
          nodeSrc: parseInt(parts[0]),
          nodeDst: parseInt(parts[1]),
          type: parts[2],
          bandWidth: parts[3],
          sendTime: parseInt(parts[4]),
          laterTime:new Date(parseInt(parts[4])),
        };
      });

      if (newData.length > 0) {
        // 过滤掉已经处理过的数据
        const uniqueNewData = newData.filter(item => !this.processedData.has(item.sendTime));

        if (uniqueNewData.length > 0) {
          // 写入输出表格
          this.dataStreamOutputData = this.dataStreamOutputData.concat(uniqueNewData);
          localStorage.setItem('dataStreamOutputData', JSON.stringify(this.dataStreamOutputData));

          // 标记为已处理
          uniqueNewData.forEach(item => this.processedData.add(item.sendTime));
        }
      }
    },
    // 获取数据定时器
    dataStreamRefreh() {
      if (this.intervalId != null) {
        return;
      }
      // 计时器为空，获取数据
      this.intervalId = setInterval(() => {
        this.getDataStreamForServer();
      }, 1000);
    },
    // 停止定时器
    clear() {
      clearInterval(this.intervalId); // 清楚计时器
      this.intervalId = null;
    },
    // 计时结束
    hilarity(index) {
      this.dataStreamOutputData.splice(index, 1);
      // 强制更新组件
      localStorage.setItem('dataStreamOutputData', JSON.stringify(this.dataStreamOutputData));
      this.forceRefresh  = new Date().getTime();
    },
    // 在页面加载后从本地存储中恢复数据
    checkAndRemoveExpiredData() {
      const outputDataJSON = localStorage.getItem('dataStreamOutputData');
      if (outputDataJSON) {
        const outputData = JSON.parse(outputDataJSON);

        const currentTime = new Date();
        outputData.forEach((data) => {
          if (currentTime > new Date(data.laterTime)) {
            const index = outputData.indexOf(data);
            if (index!== -1) {
              outputData.splice(index, 1);
            }
          }
        });
        localStorage.setItem('dataStreamOutputData', JSON.stringify(outputData));
        this.dataStreamOutputData = outputData;
      }
    },
    // 输入约束规则
    getInputType(prop) {
      if (["nodeSrc", "nodeDst", "sendTime"].includes(prop)) {
        return "number";
      }
      return "text";
    },
    // 检测节点触发函数
    async detectNodes() {
      // 检测节点后端url:/api/detect_nodes
      const res = await axios.post("/api/detect_nodes", {}, {});
      if (res.data.code === 200) {
        const data = JSON.parse(res.data.message);
        this.nodeInfoData = data;
      } else {
        this.$notify.error("检测节点失败");
        console.log(res.data.message);
      }
    },
    // 数据流：注入数据按钮触发函数
    // 将dataStreamInputData数据发送给后端：/api/data_insert
    async insertData() {
      // 将 nodeA、nodeB 和 sendTime 转换为整数类型
      const insertData = {...this.dataStreamInputData[0]};
      insertData.nodeSrc = parseInt(insertData.nodeSrc);
      insertData.nodeDst = parseInt(insertData.nodeDst);
      insertData.sendTime = parseInt(insertData.sendTime);
      // 注入数据发送给后端，不直接显示注入数据
      const res = await axios.post("/api/data_insert", insertData, {});
      if (res.data.code === 200) {
        this.$notify.success("注入成功");
      } else {
        this.$notify.error("注入错误");
        console.log(res.data.message);
      }
    },
    // 从csv文件中读取数据发送给后端
    insertDataFromFile() {
      // 打开文件选择器
      const input = document.createElement("input");
      input.type = "file";
      input.accept = ".csv";
      input.addEventListener("change", async (event) => {
        const file = event.target.files[0];
        if (file) {
          try {
            const text = await this.readFileText(file);
            const lines = text.split('\n');
            for (const line of lines) {
              if(line) {
                const [begin,nodeSrc,nodeDst,type,bandWidth,sendTime] = line.split(',');
                await new Promise(resolve => {
                  setTimeout(() => {
                    const result = {
                      "nodeSrc":  parseInt(nodeSrc),
                      "nodeDst":  parseInt(nodeDst),
                      "type": type,
                      "bandWidth": bandWidth,
                      "sendTime": parseInt(sendTime)
                    };
                    // 将数据发送到后端的操作
                    this.sendInsertData(result);
                    resolve();
                  }, parseInt(begin) * 1000);
                });
              }
            }
            this.$notify.success(file.name + "文件数据读取发送完毕！");
          } catch (error) {
            console.error("读取文件出错", error);
          }
        }
      })
      input.click();
    },
    //  将注入数据发送至后端
    async sendInsertData(insertData) {
      // const laterTime = new Date(new Date().getTime() + insertData.sendTime * 1000);
      // insertData.laterTime = laterTime;
      // insertData.sendTime = laterTime.getTime();
      // this.dataStreamOutputData.push(insertData);
      // console.log(this.dataStreamOutputData);
      // localStorage.setItem('dataStreamOutputData', JSON.stringify(this.dataStreamOutputData));
      const res = await axios.post("/api/data_insert", insertData, {});
      if (res.data.code === 200) {
        // 获取当前时间和 sendTime 秒以后的时间
        // const currentTime = new Date();
        const laterTime = new Date(new Date().getTime() + insertData.sendTime * 1000);
        insertData.laterTime = laterTime;
        // 注入数据成功：将注入数据写入output表单，同时开始计时
        // 深拷贝数据并添加到 output 表单

        insertData.sendTime = laterTime.getTime();
        this.dataStreamOutputData.push(JSON.parse(JSON.stringify(insertData)));
        // 将当前的输出数据保存到本地存储
        localStorage.setItem('dataStreamOutputData', JSON.stringify(this.dataStreamOutputData));
      } else {
        this.$notify.error("注入错误");
        console.log(res.data.message);
      }
    },
    // 读取文件内容
    async readFileText(file) {
      return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => resolve(reader.result);
        reader.onerror = reject;
        reader.readAsBinaryString(file);
      });
    },
    // 数据流注入：随机注入按钮触发函数
    async randomInsertData() {
      // 随机注入按钮
      const res = await axios.post("/api/random_insert", {}, {});
      if (res.data.code === 200) {
        this.$notify.success("随机注入成功")
      } else {
        this.$notify.error("随机注入失败");
        console.log(res.data.message);
      }
    },
    // 数据流注入：扫描按钮触发函数
    async scanNodes() {
      const res = await axios.post("/api/scan_node", {}, {});
      if (res.data.code === 200) {
        this.$notify.success("扫描成功")
      } else {
        this.$notify.error("扫描失败");
        console.log(res.data.message);
      }
    }
  }
}
</script>


<style scoped>
.button_class{
  float: right;
  margin-top: -10px;
  margin-bottom: 10px
}

.first-row-border {
  border: 2px solid red;
}
.other-row-border {
  border: 1px solid black;
  margin-top: 20px;
}
</style>
