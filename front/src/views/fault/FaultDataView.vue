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
        <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">数据流</p>
        <el-button type="primary" @click="detectNodes()" class="button_class">注入数据</el-button>
        <el-button type="warning" @click="detectNodes()" class="button_class" style="margin-right: 5px">随机注入</el-button>
        <el-button type="success" @click="detectNodes()" class="button_class">扫描</el-button>
        <div>
          <div>
            <el-table :data="dataStreamInputData"
                      max-height="250"
                      stripe highlight-current-row @cell-click="cellClick">
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
                      @blur="removeClass"
                      @change="handleEdit(item.prop, scope.$index, scope.row)"
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
      dataStreamOutputData: DataStreamOutputData,
      dataStreamColumn: DataStreamColumn,
    }
  },
  mounted() {

  },
  methods: {
    // 表格对象可编辑处理
    cellClick(row, column, cell, event) {
      for(let i=0; i<document.getElementsByClassName('current-cell').length;i++){
        document.getElementsByClassName('current-cell')[i].classList.remove('current-cell');
      }
      cell.classList.add("current-cell");
    },
    removeClass(){
      document.getElementsByClassName('current-cell')[0].classList.remove('current-cell');
    },
    handleEdit() {},
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
    resetForm(formName) {
      this.$refs[formName].resetFields();
    },
    submitForm(formName) {
      this.$refs[formName].validate(async (valid) => {
        if (valid) {
          // 校验通过，注入故障
          // 将 bandWith 转换为字符串类型
          this.faultDataForm.bandWidth = String(this.faultDataForm.bandWidth);

          // 将 nodeA、nodeB 和 sendTime 转换为整数类型
          this.faultDataForm.nodeA = parseInt(this.faultDataForm.nodeA);
          this.faultDataForm.nodeB = parseInt(this.faultDataForm.nodeB);
          this.faultDataForm.sendTime = parseInt(this.faultDataForm.sendTime);
          console.log(this.faultDataForm);
          const res = await axios.post("/api/data_insert", this.faultDataForm, {});
          if (res.data.code === 200) {
            await this.$router.push({
              name: 'FaultDataView',
            })
          } else {
            this.$notify.error("注入错误");
            console.log(res.data.message);
          }
        } else {
          this.$notify.error("error submit!!")
          return false;
        }
      });
    },
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