## 表说明

```
GET /?tn=48021271_10_hao_pg HTTP/1.1
Host: www.baidu.com
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
```

源ip和目的ip从ip头部获取

比如抓到上面这个http request，那需要往表requests存：

| id   | src_ip | dest_ip                |
| ------- | ------ | :--------------------- |
| 自己设定一个id，比如123 | 源ip | 目的ip |

往request_header表存：

| req_id | header_key | header_value |
| ---- | ------ | :--------------------- |
|123|Host|www.baidu.com|
|123|Connection|keep-alive|
|123|Upgrade-Insecure-Requests|1|
|123|User-Agent|Mozilla/5.0 (Windows NT 10.0; Win64; x64)|

GET / HTTP/1.1
首部行1\r\n
首部行2\r\n
...
首部行n\r\n
\r\n
payload