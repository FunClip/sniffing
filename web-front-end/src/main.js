import Vue from "vue";
import App from "./App";
import router from "./router/";
import BlackDashboard from "./plugins/blackDashboard";
import i18n from "./i18n";
import BootstrapVue from 'bootstrap-vue';
import echarts from 'echarts' //引入echarts
import BaiduMap from 'vue-baidu-map';

Vue.use(BaiduMap,{
  v:'3.0',
  ak:"H7QYQGLBHfRfHPsU2hDifvy0p6UTYt8N"
});
Vue.prototype.$echarts = echarts;
Vue.use(BootstrapVue);
Vue.use(BlackDashboard);

/* eslint-disable no-new */
new Vue({
  router,
  i18n,
  render: h => h(App)
}).$mount("#app");