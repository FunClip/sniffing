import MyLayout from "@/layout/MyLayout.vue";
// GeneralViews

// pages
import Requests from "@/pages/Requests.vue";
import FrequenceSrcRank from "@/pages/FrequenceSrcRank.vue";
import FrequenceHostRank from "@/pages/FrequenceHostRank.vue";
import Interests from "@/pages/Interests.vue";
import Maps from "@/pages/Maps.vue";

const routes = [
  {
    path: "/",
    component: MyLayout,
    redirect: "/requests",
    children: [
      {
        path: "requests",
        name: "requests",
        component: Requests
      },
      {
        path: "frequence_src_rank",
        name: "frequence_src_rank",
        component: FrequenceSrcRank
      },
      {
        path: "frequence_host_rank",
        name: "frequence_host_rank",
        component: FrequenceHostRank
      },
      {
        path: "interests",
        name: "interests",
        component: Interests
      },
      {
        path: "maps",
        name: "maps",
        component: Maps
      }
    ]
  },
];

/**
 * Asynchronously load view (Webpack Lazy loading compatible)
 * The specified component must be inside the Views folder
 * @param  {string} name  the filename (basename) of the view to load.
function view(name) {
   var res= require('../components/Dashboard/Views/' + name + '.vue');
   return res;
};**/

export default routes;
