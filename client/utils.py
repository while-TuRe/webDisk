from get_conf import get_conf
def loadConfig(config_path):
    f = open(config_path, 'r', encoding='gbk')
    txt = f.read()
    global config
    config = get_conf(txt)
    print(config)

config = loadConfig('./files/config.txt')

log = open(config['local']['log_file'])
def getNowTime():
    return datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S')