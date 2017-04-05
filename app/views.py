import os
import requests, requests.auth
from app import api, db, app, models
from uuid import uuid4
from flask import Flask, render_template, request, redirect, jsonify, abort
from flask_restful import Resource, Api, fields, marshal, reqparse
from sqlalchemy import exists


# app = Flask(__name__)
# api = Api(app)
plant_fields = {
    'Name':fields.String,
    'Serial_num':fields.Integer,
    'WaterLevel': fields.String,
    'Soil' : fields.String,
    'uri':fields.Url('plant')
    }
parser = reqparse.RequestParser()
owner = models.User.query.first()

# print(owner)
# plants = [{'Name':"Cactus", "WaterLevel":"EMPTY", "Soil":0,'uri':'/Plants/Cactus', 'Serial_#':1},
# {'Name':"Avacado", "WaterLevel":"FULL",
# "Soil":100, 'uri':'/Plants/Avacado','Serial_#':2}]

# plants = [plant.todict() for plant in models.Plant.query.all()]

# def abort_call(Name):
#     if Name not in plants:
#         abort(404, message='Plant {} does not exist'.format(Name))

# parser = reqparse.RequestParser()
# parser.add_argument("Plant")

class Plant(Resource):

    def __init__(self):
        self.reqparse = reqparse.RequestParser()
        self.reqparse.add_argument('Name', type=str, location='json')
        self.reqparse.add_argument('WaterLevel', type=str, location='json')
        self.reqparse.add_argument('Soil', type=int, location='json')
        self.reqparse.add_argument('Serial_num', type=int, location='json')


        super(Plant, self).__init__()


    def get(self, Name):
        # plant = [plant for plant in plants if plant['Name'] == Name].pop()
        plant = models.Plant.query.filter(models.Plant.name == Name).first()
        # abort_call(plant)
        if plant:
            return {'plant':marshal(plant.todict(), plant_fields)}
        else:
            abort(404)

    def put(self, Name):
        args = self.reqparse.parse_args()
        # print(args['Name'])
        # plant_change = [plant for plant in plants if plant['Name'] == Name].pop()
        plant_change = models.Plant.query.filter(models.Plant.Serial_num == args['Serial_num']).first()

        if plant_change:
            plant_change.name = args['Name']
            plant_change.WaterLevel = args['WaterLevel']
            plant_change.Soil = args['Soil']
            plant_change.uri = '/Plants/{}'.format(args['Name'])

            db.session.commit()

            # plant = {'Name':args['Name'],
            #          'WaterLevel': args['WaterLevel'],
            #          'Soil': args['Soil'], 'uri':"/Plants/{}".format(args['Name'])}

            # if plant['WaterLevel']:
            #     plant['WaterLevel'] = "FULL"
            # else:
            #     plant['WaterLevel'] = "EMPTY"




            return {'plant':marshal(plant_change.todict(), plant_fields)}, 202
        else:
            abort(404)


    def delete(self, Name):
        args = self.reqparse.parse_args()
        print (args)

        # plant = [plant for plant in plants if plant['Name'] == Name].pop()
        plant = models.Plant.query.filter(models.Plant.Serial_num==args['Serial_num']).first()
        plant = 0;
        if plant:
            db.session.delete(plant)
            db.session.commit()
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
        self.reqparse.add_argument('Serial_num', type=int, location='json', required=True)




        super(PlantList, self).__init__()

    def get(self):
        # print([plant.todict() for plant in models.Plant.query.all()])
        return {'plants': [marshal(plant.todict(), plant_fields) for plant in models.Plant.query.all()]}


    def post(self):
        args = self.reqparse.parse_args()
        print(args)
        # args = self.reqparse.parse_args()
        # print("Args: {}".fromat(args))
        plant = models.Plant(name=args['Name'],
                             Serial_num=args['Serial_num'],
                             WaterLevel=args['WaterLevel'],
                             Soil=args['Soil'],
                             uri="/Plants/{}".format(args['Name']),
                             owner=owner)
        print(plant.Serial_num)
        # ret = models.Plant.query(exists(), where(models.Plant.Serial_num==plant.Serial_num)).scalar()
        ret = db.session.query(exists().where(models.Plant.Serial_num==args['Serial_num'])).scalar()
        print(ret)

        if ret:
            return {"response":"Plant already exists"}, 208

        else:
            db.session.add(plant)
            db.session.commit()

        return {'plants': marshal(plant.todict(), plant_fields)}, 201




@app.route('/', methods=['GET', 'POST'])
def home():

    return render_template('index.html')


api.add_resource(PlantList, '/Plants')
api.add_resource(Plant, '/Plants/<Name>')



# if __name__ == '__main__':

#     # app.secret_key = os.urandom(24)
#     app.run(debug=True, port=5000)
#     # app.run()
