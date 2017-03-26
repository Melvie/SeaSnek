from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask import Flask, render_template, request, redirect, jsonify, abort
from flask_restful import Resource, Api, fields, marshal, reqparse


app = Flask(__name__)

app.config.from_object('config')
db = SQLAlchemy(app)
api = Api(app)


from app import views, models


