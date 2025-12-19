from stm32_protocol import STM32Serial

import json
from flask import jsonify, request
from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
	return 'Hello, World!\n'

list_temp = []
list_press = []
temp_index = 0
press_index = 0
temp = 0.0
press = 0.0

stm = STM32Serial(port="/dev/ttyS0", baudrate=115200)

@app.route('/api/temp/', methods=['GET', 'POST'])
def api_temp():
   global stm
   global list_temp
   global temp_index
   global temp

   if request.method == "POST":
      temp = stm.get_temperature()
      list_temp.append(temp)
      temp_index += 1
      return '', 200

   if request.method == "GET":
      resp = {
         "temperature" : list_temp,
      }
      return jsonify(resp), 200


@app.route('/api/temp/<int:index>', methods=['GET', 'DELETE'])
def api_temp_index(index):
   global list_temp
   global temp_index

   if (index  > temp_index):
      abort(404)

   if request.method == 'GET':
      resp = {
         "index" : index,
         "temperature" : list_temp[index]
      }
      return jsonify(resp), 200

   if request.method == 'DELETE':
      list_temp.pop(index)
      temp_index -= 1
      return '', 200


@app.route('/api/press/', methods=['GET', 'POST'])
def api_press():
   global stm
   global list_press
   global press_index
   global press

   if request.method == "POST":
      temp = stm.get_pressure()
      list_press.append(temp)
      press_index += 1
      return '', 200

   if request.method == "GET":
      resp = {
         "pression" : list_press,
      }
      return jsonify(resp), 200


@app.route('/api/press/<int:index>', methods=['GET', 'DELETE'])
def api_press_index(index):
   global list_press
   global press_index

   if (index  > press_index):
      abort(404)

   if request.method == 'GET':
      resp = {
         "index" : index,
         "pression" : list_press[index]
      }
      return jsonify(resp), 200

   if request.method == 'DELETE':
      list_press.pop(index)
      press_index -= 1
      return '', 200


@app.route('/api/scale/', methods=['GET'])
def api_scale():

   global stm

   if request.method == "GET":
      K = stm.get_k()
      resp = {
         "k" : K,
      }
      return jsonify(resp), 200


@app.route('/api/scale/<int:index>', methods=['POST'])
def api_scale_index(index):

   global stm

   if (index < 1000 or index > 9999):
      return jsonify({"error" : "value of K have to be 4 digits !!!!"}), 400

   if request.method == 'POST':
      stm.set_k(index)
      resp = {
         "new scale" : index,
      }
      return jsonify(resp), 200


@app.route('/api/angle/', methods=['GET'])
def api_angle():

   global stm

   if request.method == 'GET':
      angle = stm.get_angle()
      return jsonify({"angle" : angle}), 200
