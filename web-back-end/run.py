from flask import Flask, jsonify, request
from flask_cors import CORS
import utils

app = Flask(__name__)
CORS(app, supports_credentials=True)


@app.route("/get_total", methods=["GET"])
def get_total():
    return jsonify(count=utils.query_counts())

@app.route("/get_request/<int:id>", methods=["GET"])
def get_request_info(id):
    src_ip, dest_ip, datas = utils.query_request(id)
    return jsonify(src_ip=src_ip, dest_ip=dest_ip, datas=datas)

@app.route("/get_src_ip_rank", methods=["GET"])
def get_src_ip_rank():
    name, counts = utils.query_src_rank()
    return jsonify(name=name,counts=counts)

@app.route("/get_dest_ip_rank", methods=["GET"])
def get_dest_ip_rank():
    name, counts = utils.query_host_rank()
    return jsonify(name=name,counts=counts) 

@app.route("/get_interests", methods=["GET"])
def get_interests():
    names, series = utils.analysis_interests()
    return jsonify(names=names,series=series)

@app.route("/get_gps_points", methods=["GET"])
def get_gps_points():
    points = utils.gen_gps_point()
    return jsonify(points=points)

if __name__ == '__main__':
    app.run()