<template>
    <card>
        <div id="map1"></div>
    </card>
</template>
<script>
import {getGPSPoints} from "@/service";
export default {
    methods: {
        marker: function(Gpoints) {
            var map = new BMap.Map("map1");
            map.enableScrollWheelZoom();
            var point = new BMap.Point(116.404, 39.915);
            map.centerAndZoom(point, 4);

            var pt = null;
            let points = []
            for(var i = 0; i < Gpoints.length; i++){
                pt = new BMap.Point(Gpoints[i][1], Gpoints[i][0]);
	            points.push(pt);
            }
            var markerClusterer = new BMapLib.MarkerClusterer(map);
            let translateCallback = function (data){
                console.log(data.status)
                if(data.status === 0) {
                    for (var i = 0; i < points.length; i++) {
                        markerClusterer.addMarker(new BMap.Marker(points[i]));
                    }
                }
            }
            var convertor = new BMap.Convertor();
            let uni_points = []
            for(let i = 0; i < points.length; i++)
            {
                if(i % 10 === 0) {
                    convertor.translate(uni_points, 1, 5, translateCallback);
                    uni_points = []
                }
                uni_points.push(points[i]);
            }
            if(uni_points.length != 0)
                convertor.translate(points, 1, 5, translateCallback);
        }
    },
    mounted() {
        getGPSPoints().then(res => {
            let points = res.data.points;
            this.marker(points)
        })
    }
}
</script>
<style scoped>
    #map1 { height:700px; width:1200px; margin:20px }
</style>
