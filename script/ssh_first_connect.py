import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys

'''
使用python使用spawn执行ssh命令
使用spawn的原因是需要使用windows的openssh的proxyjump来实现网络跳转
使用python的原因是ssh需要输入密码，windows上没有很好的这种工具，有这种工具的并不使用openssh
pkill -f "name" 是杀死所有带name名字的进程
'''

need_password = sys.argv[1]
need_password_int = int(need_password)
username = sys.argv[2]
host = sys.argv[3]
password = sys.argv[4]

child = wexpect.spawn(f'ssh {username}@{host} "echo cmd_has_exec")

print("connect ssh！")

for i in range(need_password_num):
    try:
        # 第一次连接需要同意一次本地连接
        print('expect continue connecting')
        child.expect('continue connecting', timeout=5)
        child.sendline('yes')
        print("send yes")    
    except wexpect.EOF:
        print("ssh end")
    except wexpect.TIMEOUT:
        print("wait timeout")
        
    try:
        print('expect password')
        child.expect('password', timeout=5)
        child.sendline(password)
        print("sended password")        
    except wexpect.EOF:
        print("ssh end")
    except wexpect.TIMEOUT:
        print("wait timeout")

# sendline在后台执行，如果此时close会导致ssh命令不一定执行
# cmd_has_exec表明检测到后命令已经被执行了
# 如果命令马上就可以执行完，会检测不到echo cmd_has_exec，但也不需要检测了

print(child.read())
child.close()