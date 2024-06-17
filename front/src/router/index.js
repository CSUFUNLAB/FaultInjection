import Vue from 'vue'
import VueRouter from 'vue-router'
/*import HomeView from '../views/HomeView.vue'*/
import Layout from '../views/Layout.vue'

Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'Layout',
    component: Layout,
    redirect: '/faultDataView',
    children: [
      {
        path: 'faultDataView',
        name: 'FaultDataView',
        component: () => import('@/views/fault/FaultDataView.vue')
      },
      {
        path: 'faultDataListView',
        name: 'FaultDataListView',
        component: () => import('@/views/fault/FaultDataListView.vue')
      },
      {
        path: 'faultInsertView',
        name: 'FaultInsertView',
        component: () => import('@/views/fault/FaultInsertView.vue')
      }

    ]
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
