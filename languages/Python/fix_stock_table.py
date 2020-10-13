#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
 由于股票的输入数据可能有SH/SZ，需要删除
"""
import os
import pandas as pd

# 从当前config_tdx类中加载配置文件
import config_tdx as config

input_file = os.path.join(config.configs['root_dir'], 'BIG_20170930.csv')
output_file = os.path.join(config.configs['root_dir'], 'BIG_20170930_out.csv')
df = pd.read_csv(input_file, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'', encoding='utf-8-sig')
df['Symbol'] = df['Symbol'].str[0:6]
df['InstrumentID'] = df['InstrumentID'].str[0:6]
df.to_csv(output_file, index=None)


input_file = os.path.join(config.configs['root_dir'], 'SMALL_20170930.csv')
output_file = os.path.join(config.configs['root_dir'], 'SMALL_20170930_out.csv')
df = pd.read_csv(input_file, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'', encoding='utf-8-sig')
df['Symbol'] = df['Symbol'].str[0:6]
df['InstrumentID'] = df['InstrumentID'].str[0:6]
df.to_csv(output_file, index=None)

