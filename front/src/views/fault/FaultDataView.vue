<template>
  <div>
    <div>
      <el-card>
        <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">节点信息</p>
        <el-button type="primary" @click="detectNodes()" class="button_class">检测节点</el-button>
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
        <el-button type="warning" @click="randomInsertData()" class="button_class" style="margin-right: 5px">随机注入</el-button>
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
          <div class="other-row-border">
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
                <template v-slot="scope" >
                  <el-statistic
                      v-if="item.prop === 'sendTime'"
                      @finish="hilarity(scope)"
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
    }
  },
  mounted() {
    // 在页面加载后从本地存储中恢复数据
    // this.checkAndRemoveExpiredData();
  },
  methods: {
    // 计时结束
    hilarity(scope) {
      // 计时结束，从输出框中删除数据
      this.dataStreamOutputData.splice(scope.index, 1);
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

      // 注入数据成功：将注入数据写入output表单，同时开始计时
      // 深拷贝数据并添加到 output 表单
      const res = await axios.post("/api/data_insert",insertData, {});
      if (res.data.code === 200) {
        // 获取当前时间和 sendTime 秒以后的时间
        // const currentTime = new Date();
        // const laterTime = new Date(currentTime.getTime() + insertData.sendTime * 1000);
        // insertData.currentTime = currentTime;
        // insertData.laterTime = laterTime;
        // 注入数据成功：将注入数据写入output表单，同时开始计时
        // 深拷贝数据并添加到 output 表单
        insertData.sendTime =  Date.now() + 1000 * insertData.sendTime;
        this.dataStreamOutputData.push(JSON.parse(JSON.stringify(insertData)));
        // 将当前的输出数据保存到本地存储
        // localStorage.setItem('dataStreamOutputData', JSON.stringify(this.dataStreamOutputData));
      } else {
        this.$notify.error("注入错误");
        console.log(res.data.message);
      }
    },
    // 数据流注入：随机注入按钮触发函数
    randomInsertData() {

    },
    // 数据流注入：扫描按钮触发函数
    scanNodes() {

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