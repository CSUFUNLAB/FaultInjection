import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys

need_password = sys.argv[1]
need_password_num = int(need_password)
username = sys.argv[2]
host = sys.argv[3]
password = sys.argv[4]

child = wexpect.spawn(f'ssh {username}@{host} "echo init"')

print("connect ssh！")

# 第一次连接需要同意一次本地连接
child.expect('continue connecting', timeout=10)
child.sendline('yes')

for i in range(need_password_num):
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

print(child.read())
 
print("cmd has exec！")

child.close()