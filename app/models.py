from app import db

class User(db.Model):

    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(120), index=True, unique=True)
    email = db.Column(db.String(120), index=True, unique=True)
    plants = db.relationship('Plant', backref='owner', lazy='dynamic')


    def __repr__(self):
        return '<User %r>' %(self.name)



class Plant(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    Serial_num = db.Column(db.Integer, index=True, unique=True)
    name = db.Column(db.String(64), index=True)
    WaterLevel = db.Column(db.String(64), index=True)
    Soil = db.Column(db.Integer, index=True)
    uri = db.Column(db.String(64), index=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))

    def __repr__(self):
        return '<Plant %r>' %(self.name)


    def todict(self):
        #ignores user_id for now

        return {'Name':self.name, 'Serial_#':self.Serial_num, 'WaterLevel':self.WaterLevel, 'Soil':self.Soil, 'uri':self.uri}

