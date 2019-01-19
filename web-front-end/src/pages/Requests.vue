<template>
  <div>
    <card>
      <div slot="header" class="card-header-primary" align="center">
        <br>
        <h2>Http Request包报头 #{{currentPage}}</h2>
      </div>
      <div class="row">
        <div class="col-3"></div>
        <div class="col-6">
          <p>源IP：{{src_ip}}  目的IP：{{dest_ip}}</p>
        </div>
        <div class="col-3"></div>
      </div>
      <hr style="filter: alpha(opacity=100,finishopacity=0,style=3)" width="80%" color="#6f5499" size="3" />
      <div class="row">
        <div class="col-3"></div>
        <div class="col-6">
          <base-table :data="tableData" :columns="columns">
            <template slot="columns">
              <th class="text-left">Key</th>
              <th class="text-left">Value</th>
            </template>
            <template slot-scope="{row}">
              <td>{{row.key}}</td>
              <td>{{row.value}}</td>
            </template>
          </base-table>
        </div>
        <div class="col-3"></div>
      </div>
      <hr style="filter: alpha(opacity=100,finishopacity=0,style=3)" width="80%" color="#6f5499" size="3" />
      <div slot="footer" class="row">
        <div class=col-4></div>
        <div class=col-4>
          <b-pagination size="lg" :total-rows="500" v-model="currentPage"></b-pagination>
        </div>
        <div class=col-4></div>
      </div>
    </card>
  </div>
</template>
<script>
import BaseTable from "@/components/BaseTable.vue";
import {getRequestInfo} from "@/service";
export default {
  components: {
    BaseTable
  },
  data() {
    return {
      currentPage: 1,
      columns: ['key', 'value']
    }
  },
  computed:{
    setValue: function() {
      res = getRequestInfo(this.currentPage);
      this.src_ip = res.src_ip;
      this.dest_ip = res.dest_ip;
      this.tableData = res.headers;
    }
  },
  mounted() {
    res = getRequestInfo(this.currentPage);
    this.src_ip = res.src_ip;
    this.dest_ip = res.dest_ip;
    this.tableData = res.headers;
  }
}
</script>
