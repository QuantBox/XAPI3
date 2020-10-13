from flask_wtf import FlaskForm
from wtforms.fields import StringField, SubmitField
from wtforms.validators import InputRequired


class LoginForm(FlaskForm):
    """Accepts a nickname and a room."""
    name = StringField('Name', validators=[InputRequired()])
    room = StringField('Room', validators=[InputRequired()])
    submit = SubmitField('Enter Chatroom')
