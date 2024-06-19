<template>
  <div>
    <el-container class="home-container">
      <el-header>
        <div>
          <img src="@/assets/logo.png" alt="" height="50px" />
          <span>故障分析平台</span>
        </div>
        <!--        <el-dropdown size="medium">-->
        <!--          <span class="el-dropdown-link" style="cursor: pointer; color: white">-->
        <!--            {{ user.name }}<i class="el-icon-arrow-down el-icon&#45;&#45;right"></i>-->
        <!--          </span>-->
        <!--          <el-dropdown-menu slot="dropdown" style="margin-top: 10px">-->
        <!--            <el-dropdown-item><div style="width: 50px; text-align: center;" @click="logout">退出</div></el-dropdown-item>-->
        <!--          </el-dropdown-menu>-->
        <!--        </el-dropdown>-->
      </el-header>
      <el-container>
        <el-aside width="200px">
          <el-menu :default-active="$route.path" router class="el-menu-demo"
                   background-color="#545c64"
                   text-color="#fff"
                   active-text-color="#ffd04b"
                   :style="{ 'min-height': 'calc(100vh - 60px)' }">
            <el-menu-item index="/faultInsertView">
              <i class="el-icon-s-home"></i>
              <span slot="title">故障注入</span>
            </el-menu-item>
            <el-menu-item index="/faultDataView">
              <i class="el-icon-s-home"></i>
              <span slot="title">数据注入</span>
            </el-menu-item>
            <el-menu-item index="/faultDataListView">
              <i class="el-icon-s-home"></i>
              <span slot="title">数据显示</span>
            </el-menu-item>
            <el-menu-item index="/faultDiagnosisView">
              <i class="el-icon-s-home"></i>
              <span slot="title">故障诊断</span>
            </el-menu-item>
          </el-menu>
        </el-aside>
        <el-main style="flex-grow: 1;">
          <div>
            <router-view />
          </div>
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script>
export default {
  name: "Layout",
  methods: {
  }
}
</script>


<style lang="less" scoped>
.home-container {
  height: 100%;
}
.el-header {
  background-color: #373d41;
  display: flex; //设置显示为flex布局
  justify-content: space-between; //设置为flex左右布局
  padding-left: 0; //左内边距为0（Logo贴左边）
  align-items: center; //元素上下居中（防止右边按钮贴上下边）
  color: #fff;
  font-size: 20px;
  > div {
    //内嵌的div样式
    display: flex;
    align-items: center; //Logo和文字上下居中
    span {
      margin-left: 15px; //文字左侧设置间距，防止与Logo紧贴
    }
  }
}
.el-aside {
  background-color: #545c64;
}
.el-main {
  background-color: #eaedf1;
}

</style>