<template>
  <div class="wrapper">
    <side-bar data-background-color="darkblue">
      <template slot="links">
        <sidebar-link to="/requests" :name="$t('sidebar.requests')"/>
        <sidebar-link to="/frequence" :name="$t('sidebar.frequence')"/>
        <sidebar-link to="/interests" :name="$t('sidebar.interests')"/>
        <sidebar-link to="/maps" :name="$t('sidebar.maps')"/>
      </template>
    </side-bar>
    <div class="main-panel">
      <my-content @click.native="toggleSidebar">

      </my-content>
    </div>
  </div>
</template>
<style lang="scss">
</style>
<script>
import MyContent from "./dashboard/Content.vue";
export default {
  components: {
    MyContent
  },
  methods: {
    toggleSidebar() {
      if (this.$sidebar.showSidebar) {
        this.$sidebar.displaySidebar(false);
      }
    }
  }
};
</script>
