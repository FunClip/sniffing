import axios from 'axios'

axios.defaults.headers.post['Content-Type'] = 'application/json; charset=utf-8';

const path = "http://118.228.170.83:5000"

export const getTotal = () => {
    return axios.get(path + "/get_total")
}

export const getRequestInfo = (id) => {
    return axios.get(path + '/get_request/' + id)
}

export const getSrcIPRank = () => {
    return axios.get(path + '/get_src_ip_rank')
}

export const getDestIPRank = () => {
    return axios.get(path + '/get_dest_ip_rank')
}

export const getInterests = () =>{
    return axios.get(path + '/get_interests')
}

export const getGPSPoints = () =>{
    return axios.get(path + '/get_gps_points')
}