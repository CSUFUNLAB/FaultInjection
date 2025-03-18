import wexpect  # 注意：使用 wexpect 替代 pexpect
import sys
import time

'''
使用python使用spawn执行ssh命令
使用spawn的原因是需要使用windows的openssh的proxyjump来实现网络跳转
使用python的原因是ssh需要输入密码，windows上没有很好的这种工具，有这种工具的并不使用openssh
这个python文件命令不能和scp命令名字重复，因为执行scp的时候不知道应该是执行scp.py还是windows的scp命令
'''
need_password = sys.argv[1]
need_password_num = int(need_password)
username = sys.argv[2]
host = sys.argv[3]
password = sys.argv[4]
remote_file_path = sys.argv[5]
local_file_path = sys.argv[6]

print(f'scp -r {username}@{host}:{remote_file_path} {local_file_path}')

child = wexpect.spawn(f'scp ../../monitor.tar.gz {username}@{host}:/home/orangepi/')

'''
如果命令马上就退出了，可以用这个看退出前的打印
child.expect(wexpect.EOF)
print(child.before)
'''
  
print("connect ssh！")

for i in range(need_password_num):
    try:
        child.expect('password', timeout=10)
        print("requir password")
        child.sendline(password)
        print("sended password")        
    except wexpect.EOF:
        print("ssh eof")
    except wexpect.TIMEOUT:
        print("wait timeout")

# scp要等所有拷贝结束，spawn才可以关闭，用read命令执行完毕
print(child.read()) 

print("cmd has exec！")

child.close()