import os
import requests, requests.auth
from uuid import uuid4
from flask import Flask, render_template, request, redirect, jsonify, abort
from flask_restful import Resource, Api, fields, marshal, reqparse


app = Flask(__name__)
api = Api(app)
plant_fields = {
        'Name':fields.String,
        'WaterLevel': fields.String,
        'Soil' : fields.String,
        'uri':fields.Url('plant')
    }   

plants = [{'Name':"Cactus", "WaterLevel":"EMPTY", "Soil":0,'uri':'/Plants/Cactus'},{'Name':"Avacado", "WaterLevel":"MID", "Soil":100,'uri':'/Plants/Avacado'}]


def abort_call(Name):
    if Name not in plants:
        abort(404, message='Plant {} does not exist'.format(Name))

parser = reqparse.RequestParser()
parser.add_argument("Plant")

class Plant(Resource):

    def __init__(self):
        self.reqparse = reqparse.RequestParser()
        self.reqparse.add_argument('Name', type=str, location='json')
        self.reqparse.add_argument('WaterLevel', type=str, location='json')
        self.reqparse.add_argument('Soil', type=int, location='json')

        super(Plant, self).__init__()


    def get(self, Name):
        plant = [plant for plant in plants if plant['Name']==Name].pop()
        # abort_call(plant)
        if plant:
            return {'plant':marshal(plant, plant_fields)}
        else:
            abort(404)

    def put(self, Name):
        print(Name)
        args = self.reqparse.parse_args()
        print(request.json)
        print("Args: {}".format(args))
        plant_change = [plant for plant in plants if plant['Name']== Name].pop()
        if plant_change:
            for plant in plants:
                if plant['Name'] == Name:
                    del plants[plants.index(plant)]

            plant = {'Name':args['Name'],
                    'WaterLevel': args['WaterLevel'],
                    'Soil': args['Soil'], 'uri':"/Plants/{}".format(args['Name'])}

            plants.append(plant)

            return {'plant':marshal(plant, plant_fields)}, 201
        else:
            abort(404)


    def delete(self, Name):
        plant = [plant for plant in plants if plant['Name'] == Name].pop()

        if plant:
            plants.remove(plant)
            return {'Result':True}

        else:
            abort(404)

class PlantList(Resource):
    def __init__(self):
        self.reqparse = reqparse.RequestParser()
        self.reqparse.add_argument('Name', type=str, required=True,
                                   help='No plant name provided',
                                   location='json')
        self.reqparse.add_argument('WaterLevel', type=str, required=False,
                                   default='EMPTY', location='json')
        self.reqparse.add_argument('Soil', type=int, required=False,
                                   default=0, location='json')


        super(PlantList, self).__init__()

    def get(self):
        return {'plants': [marshal(plant, plant_fields) for plant in plants]}


    def post(self):
        args = self.reqparse.parse_args()

        plant = {"Name":args['Name'], "WaterLevel":args['WaterLevel'], "Soil":args['Soil'], 'uri':"/Plants/{}".format(args['Name'])}

        plants.append(plant)

        return {'plants': marshal(plant, plant_fields)}, 201




@app.route('/', methods=['GET', 'POST'])
def home():

    return render_template('index.html')


api.add_resource(PlantList, '/Plants')
api.add_resource(Plant, '/Plants/<string:Name>')



if __name__ == '__main__':

    app.secret_key = os.urandom(24)
    app.run(debug=True, port=5000)