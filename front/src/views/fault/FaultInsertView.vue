<template>
  <div>
    <el-card>
      <p style="text-align: center; color: black;margin-top: 0px; font-weight: bold; font-size: 20px"> 故障注入界面</p>
      <el-form :model="faultDataInsertForm" ref="faultDataInsertForm" label-width="100px" class="demo-dynamic">
        <el-form-item label="故障类型" prop="faultType">
          <el-select v-model="faultDataInsertForm.faultType" placeholder="请选择故障类型">
            <el-option
                style="width: 70%"
                v-for="item in options"
                :key="item.value"
                :label="item.label"
                :value="item.value">
            </el-option>
          </el-select>
        </el-form-item>
        <el-form-item label="故障参数" prop="nodeData">
          <el-input v-model="faultDataInsertForm.faultParameter" style="width: 70%"></el-input>
        </el-form-item>
        <el-form-item
            v-for="(node, index) in faultDataInsertForm.nodes"
            :label="'节点' + node.key"
            :key="node.key"
            :prop="'nodes.' + index + '.value'"
            :rules="{
              required: true, message: '节点不能为空', trigger: 'blur'
            }"
        >
          <el-input v-model="node.value" style="width: 70%"></el-input><el-button style="margin-left: 10px" @click.prevent="removeNode(node)">删除</el-button>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="submitForm('faultDataInsertForm')">注入</el-button>
          <el-button @click="addNode">新增节点</el-button>
          <el-button @click="resetForm('faultDataInsertForm')">重置</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script>
import axios from "axios";

export default {
  name: "FaultInsertView",
  data() {
    return {
      // 故障类型
      options: [{
        value: 'congest',
        label: 'congest'
      }, {
        value: 'obstacle',
        label: 'obstacle'
      }, {
        value: 'out',
        label: 'out'
      }, {
        value: 'normal',
        label: 'normal'
      }, {
        value: 'malicious',
        label: 'malicious'
      },  {
        value: 'appdown',
        label: 'appdown'
      }, {
        value: 'nodedown',
        label: 'nodedown'
      }],
      faultDataInsertForm: {
        nodes: [{
          value: '',
          key: 'A'
        }],
        faultType: '',
        faultParameter: ''
      },
    }
  },
  mounted() {
    // this.getFormData();
  },
  methods: {
    // 获取表单数据
    getFormData() {

    },
    // 表单提交
    submitForm(formName) {
      this.$refs[formName].validate(async (valid) => {
        if (valid) {
          // 数据格式转换
          let newFaultDataInsertForm = {
            "faultParameter": this.faultDataInsertForm.faultParameter,
            "faultType": this.faultDataInsertForm.faultType
          };
          // 根据nodes的长度添加node$对象
          for (let i = 0; i < this.faultDataInsertForm.nodes.length; i++) {
            let node = this.faultDataInsertForm.nodes[i];
            newFaultDataInsertForm[`node${node.key}`] = node.value;
          }
          // console.log(newFaultDataInsertForm);
          // 校验通过，注入故障
          const res = await axios.post("/api/fault_data_insert", this.faultDataInsertForm, {});
          if (res.data.code === 200) {
            this.$notify.success("注入成功");
          }
        } else {
          console.log('error submit!!');
          return false;
        }
      });
    },
    // 重置表单
    resetForm(formName) {
      this.$refs[formName].resetFields();
    },
    removeNode(item) {
      if (this.faultDataInsertForm.nodes.length === 1) {
        this.$alert('至少需要一个节点', '提示', {
          confirmButtonText: '确定',
        });
      } else {
        let index = this.faultDataInsertForm.nodes.indexOf(item)
        if (index !== -1) {
          this.faultDataInsertForm.nodes.splice(index, 1)
        }
      }
    },
    addNode() {
      // 获取当前最后一个节点的键值
      const lastKey = this.faultDataInsertForm.nodes[this.faultDataInsertForm.nodes.length - 1].key ;
      // 根据最后一个节点的键值计算下一个键值
      const nextKey = String.fromCharCode(lastKey.charCodeAt(0) + 1);
      // 添加新节点并设置键值
      this.faultDataInsertForm.nodes.push({
        value: '',
        key: nextKey
      });
    }
  },
}
</script>


<style scoped>

</style>