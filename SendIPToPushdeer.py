import socket
import urllib.request
import json
import psutil
import requests
def get_local_ip():
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)
    return local_ip
def get_all_interfaces():
 
    interfaces = psutil.net_if_addrs()
    interface_info = {}
    
    for interface, addresses in interfaces.items():
        ips = []
        for addr in addresses:
            if addr.family == socket.AF_INET:
                ips.append(addr.address)
        if ips:
            interface_info[interface] = ips
    
    return interface_info

# 获取并打印所有网卡的名称和IP地址
all_interfaces = get_all_interfaces()
sourceString  = "" 
for interface, ips in all_interfaces.items():
    for ip in ips:
        sourceString += "%s:%s------"%(interface,ip)
print(sourceString)

# 对中文字符串进行URL编码
encoded_string = requests.utils.quote(sourceString)
# 将下面URL中中括号部分（包括中括号）替换为PushDeer中生成的Key 
url = "https://api2.pushdeer.com/message/push?pushkey=[PushDeer的KEY]&text=" + encoded_string# 替换为实际的链接

success_value = "none"
while success_value != "ok":
    response = urllib.request.urlopen(url)
    html = response.read().decode("utf-8") 
    parsed_data = json.loads(html)
    result_data = json.loads(parsed_data['content']['result'][0])
    success_value = result_data['success']
    print("success字段的值:", success_value)
    
