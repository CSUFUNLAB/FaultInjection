import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys

'''
使用python使用spawn执行ssh命令
使用spawn的原因是需要使用windows的openssh的proxyjump来实现网络跳转
使用python的原因是ssh需要输入密码，windows上没有很好的这种工具，有这种工具的并不使用openssh
'''


username = sys.argv[1]
host = sys.argv[2]
password = sys.argv[3]
cmd = sys.argv[4]

print(f'ssh {username}@{host} "{cmd}"')

# 启动 SSH 命令（假设已配置 OpenSSH 客户端）
child = wexpect.spawn(f'ssh {username}@{host} "{cmd}"')

print("connect ssh！")


if True:
# 不知道为什么要循环这个命令才可以让命令执行成功
    try:
        while True:
            # 非阻塞读取输出
            child.expect('password', timeout=10)
            print("requir password")
            child.sendline(password)
            print(child.read())
    except wexpect.EOF:
        print("子进程已结束")
    except wexpect.TIMEOUT:
        print("等待超时")
else:
    child.expect('password', timeout=10)
    print("requir password")
    child.sendline(password)

print("login！")

child.close()