import MySQLdb
from geolite2 import geolite2
import pandas

connection = MySQLdb.connect(host='127.0.0.1', user='root', password='Root!!2018', db='sniffing')

def query_counts():
    cur = connection.cursor()
    sql = "SELECT COUNT(*) FROM requests"
    cur.execute(sql)
    result = cur.fetchall()
    return result[0][0]

def query_request(id):
    cur = connection.cursor()
    sql1 = "SELECT src_ip, dest_ip FROM requests WHERE id=%d" % id
    sql2 = "SELECT header_key, header_value FROM request_headers WHERE req_id=%d" % id
    cur.execute(sql1)
    result = cur.fetchall()
    src_ip, dest_ip = result[0]
    cur.execute(sql2)
    results = cur.fetchall()
    datas=[]
    for row in results:
        datas.append({'key':row[0],'value':row[1]})
    return src_ip, dest_ip, datas

def query_src_rank():
    names = []
    counts = []
    cur = connection.cursor()
    sql = "SELECT src_ip, COUNT(*) as c FROM requests GROUP BY src_ip ORDER BY c"
    cur.execute(sql)
    results = cur.fetchall()
    for row in results:
        names.append(row[0])
        counts.append(row[1])
    return names, counts

def query_host_rank():
    names = []
    counts = []
    cur = connection.cursor()
    sql = '''SELECT vt.header_value, COUNT(*) as c FROM (
            SELECT header_key, header_value FROM request_headers WHERE header_key='Host'   
                ) as vt GROUP BY header_value ORDER BY c'''
    cur.execute(sql)
    results = cur.fetchall()
    for row in results:
        names.append(row[0])
        counts.append(row[1])
    return names, counts

def analysis_interests():
    cur = connection.cursor()
    sql = "SELECT src_ip, header_value FROM requests, request_headers WHERE requests.id = request_headers.req_id AND header_key='Host'"
    cur.execute(sql)
    res = pandas.DataFrame(list(cur.fetchall()))
    names = list(res.groupby(0).count().index)
    hosts = list(res.groupby(1).count().sort_values(by=0, ascending=False).index)[0:10]
    series = []
    for host in hosts:
        num = []
        for ip in names:
            num.append(len((res.loc[res[0]==ip])[res[1]==host]))
        series.append({'name':host,'data':num})
    return names, series


def gen_gps_point():
    cur = connection.cursor()
    sql = "SELECT dest_ip FROM requests GROUP BY dest_ip"
    cur.execute(sql)
    results = cur.fetchall()
    points = []
    reader = geolite2.reader()
    for ip in results:
        data = reader.get(ip[0])
        if data is not None:
            points.append([data['location']['latitude'],data['location']['longitude']])
    return points