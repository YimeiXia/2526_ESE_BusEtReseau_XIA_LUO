import json
from flask import jsonify, request
from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"


@app.route('/api/welcome/', methods=['GET', 'POST', 'DELETE'])
def api_welcome():
    global welcome

    if request.method == 'POST':
         data = request.get_json()
         if data and "new_message" in data:
            welcome = data["new_message"]
            return '', 200
         else :
            return jsonify({ "error": "new_message not found" }), 400

    elif request.method == 'GET':
         resp = {
             "welcome" : welcome,
         }
         return jsonify(resp), 200

    elif request.method == 'DELETE':
         welcome = ""
         return '', 202

    else:
         resp = {
             "error" : "aie aie aie"
         }
         return resp, 404


@app.route('/api/welcome/<int:index>', methods=['GET', 'PUT', 'PATCH', 'DELETE'])
def api_welcome_index(index):
    global welcome

    if (index  > 19):
         abort(404)

    if request.method == 'GET':
         resp = {
             "index" : index,
             "letter" : welcome[index]
         }
         return jsonify(resp), 200

    if request.method == 'PUT':
         data = request.get_json()
         if data and "new_word" in data:
             welcome = welcome[:index] + " " + data["new_word"] + " " + welcome[index:]
             return '', 200
         else :
             return jsonify({"error": "new_word no found"}), 400

    if request.method == 'PATCH':
         data = request.get_json()
         if (data and "letter" in data) :
             if len(data["letter"]) != 1 :
                 return jsonify({"error" : "letter is not single character"}), 400
             else :
                 welcome = welcome[:index] + data["letter"] + welcome[index-1:]
                 return '', 200
         else:
             return jsonift({"error": "letter not found"}), 400

    if request.method == 'DELETE':
         welcome = welcome[:index] + welcome[index-1:]
         return '', 200


@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)
