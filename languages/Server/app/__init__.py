from flask import Flask
from flask_socketio import SocketIO

socketio = SocketIO()


def create_app(debug=False):
    """Create an application."""
    app = Flask(__name__)
    app.debug = debug
    app.config['SECRET_KEY'] = 'gjr39dkjn344_!67#'

    from .main import main as main_blueprint
    app.register_blueprint(main_blueprint)

    socketio.init_app(app)

    from .api.events import XApiNamespace
    socketio.on_namespace(XApiNamespace('/api'))
    return app

# from app.api.api import config_md, config_td
#
# md, td = config_md(), config_td()
#
# from app.socketio_queue import EmitQueue
#
# mq = EmitQueue(socketio)
