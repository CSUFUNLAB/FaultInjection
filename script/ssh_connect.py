import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys

'''
使用python使用spawn执行ssh命令
使用spawn的原因是需要使用windows的openssh的proxyjump来实现网络跳转
使用python的原因是ssh需要输入密码，windows上没有很好的这种工具，有这种工具的并不使用openssh
'''
need_read = sys.argv[1]
password_loop = sys.argv[2]
password = sys.argv[3]
cmd = sys.argv[4]

need_read_int = int(need_read)
password_loop_int = int(password_loop)

# 启动 SSH 命令（假设已配置 OpenSSH 客户端）
# 加一条echo打印，表明命令已经开始执行了
# child = wexpect.spawn(f'ssh {username}@{host} "echo cmd_has_exec; {cmd}"')

print(cmd)

child = wexpect.spawn(f'{cmd}')

#print("connect ssh！")

for i in range(password_loop_int):
    if need_read_int == 3:
        try:
            print('expect continue connecting')
            child.expect('continue connecting', timeout=5)
            child.sendline('yes')
            print("ssh yes")        
        except wexpect.EOF:
            print("ssh end")
        except wexpect.TIMEOUT:
            print("expect continue connecting wait timeout")

    try:
        child.expect('password', timeout=10)        
        child.sendline(password)
        #print("sended password")        
    except wexpect.EOF:
        print("ssh end")
    except wexpect.TIMEOUT:
        print("password wait timeout")

# sendline在后台执行，如果此时close会导致ssh命令不一定执行
# cmd_has_exec表明检测到后命令已经被执行了
# 如果命令马上就可以执行完，会检测不到echo cmd_has_exec，但也不需要检测了

if need_read_int == 0:
    try:
        child.expect('cmd_has_exec')
    except wexpect.EOF:
        print("ssh end")
    except wexpect.TIMEOUT:
        print("cmd end wait timeout")
else:
    print(child.read())
 
print("cmd has exec！")

child.close()
