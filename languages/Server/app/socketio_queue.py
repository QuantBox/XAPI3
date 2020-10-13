import queue

from threading import Lock


def background_thread(self):
    self.background_thread()


class EmitQueue(object):
    def __init__(self, sio):
        self._queue = queue.Queue()
        self._socketio = sio
        self._thread = None
        self.thread_lock = Lock()

    def emit(self, event, *args, **kwargs):
        self._queue.put((event, args, kwargs))

    def on_connect(self):
        """
        必须与socketio.on在同一线程中，所以不得不写在收到connect事件中处理
        :return:
        """
        with self.thread_lock:
            if self._thread is None:
                self._thread = self._socketio.start_background_task(background_thread, self)

    def background_thread(self):
        while True:
            if self._queue.empty():
                self._socketio.sleep(0.1)
            else:
                t = self._queue.get()
                self._socketio.emit(t[0], *t[1], **t[2])
