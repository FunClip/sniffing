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
          <b-pagination size="lg" :total-rows="totalRows" :per-page=1 v-model="currentPage" @change="setValue($event)"></b-pagination>
        </div>
        <div class=col-4></div>
      </div>
    </card>
  </div>
</template>
<script>
import BaseTable from "@/components/BaseTable.vue";
import {getRequestInfo, getTotal} from "@/service";
export default {
  components: {
    BaseTable
  },
  data() {
    return {
      currentPage: 1,
      columns: ['key', 'value'],
      src_ip: null,
      dest_ip: null,
      tableData: null,
      totalRows: null
    }
  },
  methods: {
    setValue: function(event) {
      this.currentPage = event;
      return getRequestInfo(this.currentPage).then(res =>{
      this.src_ip = res.data.src_ip;
      this.dest_ip = res.data.dest_ip;
      this.tableData = res.data.datas;
    })
    }
  },
  mounted() {
    getTotal().then(res => {this.totalRows = res.data.count})
    getRequestInfo(this.currentPage).then(res =>{
      this.src_ip = res.data.src_ip;
      this.dest_ip = res.data.dest_ip;
      this.tableData = res.data.datas;
    })
  }
}
</script>
