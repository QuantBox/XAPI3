%% Combine Event Handlers as MATLAB Local Functions
function a = mycallbacks(str) 
a = str2func(str);

%%
function global_dispatcher(varargin)
global api;
api.dispatcher(varargin);