#!/usr/bin/env python
# -*- coding: utf-8 -*-

# https://stackoverflow.com/questions/38987/how-to-merge-two-python-dictionaries-in-a-single-


def deepupdate(original, update):
    """
    Recursively update a dict.
    Subdict's won't be overwritten but also updated.
    """
    for key, value in original.items():
        if key not in update:
            update[key] = value
        elif isinstance(value, dict):
            deepupdate(value, update[key])
    return update


import config_default

configs = config_default.configs

try:
    import config_override

    configs = deepupdate(configs, config_override.configs)
except ImportError:
    pass
