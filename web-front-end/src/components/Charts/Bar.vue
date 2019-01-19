<template>
    <div>
      <div id="myChart" ></div>
    </div>
</template>
<script>
    export default {
      data () {
        return {
          msg: ''
        }
      },
      name: "bar",
      props: {
        title: String,
        series: Object
      },
      mounted () {
        this.drawLine()
      },
      methods: {
        drawLine () {
          // 基于准备好的dom, 初始化echarts
          let myChart = this.$echarts.init(document.getElementById('myChart'))
          // 绘制图表
          myChart.setOption({
            title: {},
            grid: { 
            left: '1%', 
            bottom: '5%', 
            containLabel: true 
         } ,
            tooltip: {},
            yAxis: {
              type : 'category',
              data: this.series.name,
              axisTick: {
                alignWithLabel: true
              }
            },
            xAxis : [
                {
                    type : 'value'
                }
            ],
            series: [{
              type: 'bar',
              barWidth: '50%',
              data: this.series.valure,
            }]
          })
          console.log(this.series.name)
        }
      },
      watch:{
            houseStyle:{
                handler(val,oldval){
                this.houseStyle=val
                this.drawLine ()
            },
            deep:true
         }
        }
    }
</script>