<template>
  <div>
    <el-card style="min-height: 600px">
      <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px">故障诊断界面</p>
      <el-button type="primary" @click="faultDiagnosis()">故障诊断按钮</el-button>
      <el-card style="margin-top: 10px" shadow="never">
        <p style="white-space: pre-wrap;"> {{ descriptionText }}</p>
      </el-card>
      <div style="width:100%;min-height:250px;border:solid 1px ;margin-top: 10px">
        <p style="font-size:15px; white-space: pre-wrap;" >
          {{ faultDiagnosisResult }}
        </p>
      </div>
    </el-card>
  </div>
</template>
<script>
import axios from "axios";

export default {
  name: "FaultDiagnosisView",
  data() {
    return {
      descriptionText: '这里是文字描述内容，你可以在此添加换行符\n第二行文字\n第三行文字',
      faultDiagnosisResult: '',
    }
  },
  methods: {
    // 故障检测触发函数
    async faultDiagnosis() {
      // 测试数据
      // const data = "\t这是第一行\n" +
      //     "\t这是第二行\n\t这是第三行";
      // this.faultDiagnosisResult = data;
      // 从后端接口获取数据:url = /api/get_diagnosis_result
      const res = await axios.post("/api/get_diagnosis_result", {}, {});
      if (res.data.code === 200) {
        this.faultDiagnosisResult = res.data.message;
      } else {
        this.$notify.error('获取结果出错');
        console.log(res.data.message);
      }
    },
  }
}

</script>


<style scoped>
.text-box {
  margin-top: 10px;
  border: 2px solid #ccc;
  padding: 50px;
  text-align: left;
  height: auto;
}
</style>