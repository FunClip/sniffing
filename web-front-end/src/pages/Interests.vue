<template>
  <div class="container-fluid row">
      <card>
        <h3 align="center">client 访问兴趣</h3>
        <div id="myChart1" :style="{ height: '750px'}"></div>
      </card>
  </div>
</template>
<script>
import {getInterests} from "@/service";
export default {
	methods: {
		drawLine: function(names, series) {
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
        data: names,
        axisLine: {
          lineStyle: {
            color: '#FFF'
          }
        }
    },
    textStyle: {
      color: '#FFF'
    },
    series: series
};
          myChart.setOption(option, true);
	}},
	mounted() {
        getInterests().then(res => {
            let series = []
            let names = res.data.names;
            res.data.series.forEach(element => {
                let serie = {
                name: '',
                type: 'bar',
                stack: '总量',
                label: {
                    normal: {
                        show: true,
                        position: 'insideRight'
                    }
                },
                data: []
            };
                serie.name = element.name;
                serie.data = element.data;
                series.push(serie)
            });
            this.drawLine(names, series);
        })
	}
}
</script>
