import logging.config
import os
import yaml


def setup_logging(default_path='logging.yaml', default_level=logging.INFO):
    """
    Setup logging configuration
    """
    path = default_path
    if os.path.exists(path):
        with open(path, 'rt') as f:
            config = yaml.load(f.read(), Loader=yaml.SafeLoader)
        logging.config.dictConfig(config)
    else:
        logging.basicConfig(level=default_level)
        print('the input path doesn\'t exist')


if '__main__' == __name__:
    setup_logging(default_path='logging.yaml')
    # 默认拿到root的logger
    logger = logging.getLogger()
    logging.info('jhaha', exc_info=True)
