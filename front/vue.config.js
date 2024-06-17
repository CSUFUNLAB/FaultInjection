const { defineConfig } = require('@vue/cli-service')
module.exports = defineConfig({
  transpileDependencies: true,
  devServer: {
    proxy:{
      '/api':{
        target:'http://127.0.0.1:8082',//产生跨域的地址
        changeOrigin:true,
        pathRewrite: {
          '^/api': '/api'
        }
      }
    }
  },
})
