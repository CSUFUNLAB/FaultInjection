<template>
  <div>
    <el-card>
      <p style="text-align: center">数据注入界面</p>
      <el-form :model="faultDataForm" ref="faultDataForm" label-width="100px" class="demo-ruleForm">
        <el-form-item label="节点A" prop="nodeA">
          <el-input v-model="faultDataForm.nodeA" autocomplete="off" style="width: 70%"></el-input>
        </el-form-item>
        <el-form-item label="节点B" prop="nodeB">
          <el-input v-model="faultDataForm.nodeB" autocomplete="off" style="width: 70%"></el-input>
        </el-form-item>
        <el-form-item label="带宽" prop="nodeData">
          <el-input v-model.number="faultDataForm.bandWidth" style="width: 70%"></el-input>
          <span>bps</span>
        </el-form-item>
        <el-form-item label="发送时间" prop="nodeData">
          <el-input v-model.number="faultDataForm.sendTime" style="width: 70%"></el-input>
          <span>t</span>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="submitForm('faultDataForm')">提交</el-button>
          <el-button @click="resetForm('faultDataForm')">重置</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script>

import axios from "axios";

export default {
  name: "FaultDataView",
  data() {
    return {
      faultDataForm: {
        nodeA: '',
        nodeB: '',
        bandWidth: '',
        sendTime: ''
      },

    }
  },
  mounted() {

  },
  methods: {
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

</style>