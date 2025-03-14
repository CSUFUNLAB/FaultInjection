import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys

'''
使用python使用spawn执行ssh命令
使用spawn的原因是需要使用windows的openssh的proxyjump来实现网络跳转
使用python的原因是ssh需要输入密码，windows上没有很好的这种工具，有这种工具的并不使用openssh
pkill -f "name" 是杀死所有带name名字的进程
'''

username = sys.argv[1]
host = sys.argv[2]
password = sys.argv[3]
cmd = sys.argv[4]
is_ap = False

# ap这个放了ssh公钥不需要输入密码，如果等待会导致时间浪费
# 后面要将输入密码做成n次循环，一共需要输入n次密码
if host == "192.168.12.1":
    is_ap = True

# 用root权限执行必须要输入密码
if username == 'root':
    is_ap = False

# print(f'ssh {username}@{host} "{cmd}"')

# 启动 SSH 命令（假设已配置 OpenSSH 客户端）
# 加一条echo打印，表明命令已经开始执行了
child = wexpect.spawn(f'ssh {username}@{host} "echo cmd_has_exec; {cmd}"')

print("connect ssh！")


if is_ap:
    print("is ap")
else:
    try:
        child.expect('password', timeout=10)
        print("requir password")
        child.sendline(password)
        print("sended password")        
    except wexpect.EOF:
        print("ssh end")
    except wexpect.TIMEOUT:
        print("wait timeout")

# sendline在后台执行，如果此时close会导致ssh命令不一定执行
# cmd_has_exec表明检测到后命令已经被执行了
# 如果命令马上就可以执行完，会检测不到echo cmd_has_exec，但也不需要检测了

try:
    child.expect('cmd_has_exec')
except wexpect.EOF:
    print("ssh end")
except wexpect.TIMEOUT:
    print("wait timeout")

#print(child.read())
 
print("cmd has exec！")

child.close()