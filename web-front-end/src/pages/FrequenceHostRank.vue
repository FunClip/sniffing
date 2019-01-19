<template>
  <div class="container-fluid row">
      <card>
        <h3 align="center">Host 访问量</h3>
        <div id="myChart1" :style="{ height: '750px'}"></div>
      </card>
  </div>
</template>
<script>
import {getDestIPRank} from "@/service";
export default {
	methods: {
		drawLine: function(name, value) {
          // 基于准备好的dom, 初始化echarts
          let myChart = this.$echarts.init(document.getElementById('myChart1'))
					// 绘制图表
					var option = {
    tooltip: {
        trigger: 'axis',
        axisPointer: {
            type: 'shadow'
        }
    },
    grid: {
        left: '3%',
        right: '4%',
        bottom: '3%',
        containLabel: true
    },
    xAxis: {
        type: 'value',
        boundaryGap: [0, 0.01],
        axisLine: {
          lineStyle: {
            color: '#FFF',
            width: 1.5
          }
        }
    },
    yAxis: {
        type: 'category',
        data: name,
        axisLine: {
          lineStyle: {
            color: '#FFF'
          }
        }
    },
    textStyle: {
      color: '#FFF'
    },
    series: [
        {
            type: 'bar',
            data: value
        }
    ]
};
          myChart.setOption(option, true);
	}},
	mounted() {
		getDestIPRank().then(res=> {
            let name = res.data.name;
            let value = res.data.counts;
            this.drawLine(name, value);
        })
	}
}
</script>
