import axios from 'axios'

axios.defaults.headers.post['Content-Type'] = 'application/json; charset=utf-8';

export const getRequestInfo = (id) => {
    return axios.get('/request' + id)
}

export const getSrcIPRank = () => {
    return axios.get('/get_src_ip_rank')
}

export const getDestIPRank = () => {
    return axios.get('/get_dest_ip_rank')
}

export const getInterests = () =>{
    return axios.get('/get_interests')
}
