#!/usr/bin/python3
import os
import sys
import shutil

import threading
import signal


def down_git_branch(lname,pname,rname,foldrep,branch):
    if os.path.exists(foldrep):
        shutil.rmtree(foldrep)
    os.system("git clone https://"+lname+":"+pname+"@github.com/"+lname+"/"+rname+".git "+foldrep)
    os.system("cd "+foldrep+"&& git fetch --all")
    os.system("cd "+foldrep+"&&git checkout "+branch+" || git checkout -b "+branch+" origin/clean")

def save_repo_branch_commit(lname,pname,rname,foldrep,branch,commit):
    os.system("cd "+foldrep+"&&git remote remove origin")
    os.system("cd "+foldrep+"&&git config --global user.name \""+lname+"\"")
    os.system("cd "+foldrep+"&&git config --global user.email "+lname+"@github.com")
    os.system("cd "+foldrep+"&&git remote add -f origin https://"+lname+":"+pname+"@github.com/"+lname+"/"+rname+".git")
    os.system("cd "+foldrep+"&&git checkout "+branch+" || git checkout -b "+branch+" origin/clean")
    os.system("cd "+foldrep+"&&git add -A")
    os.system("cd "+foldrep+"&&git commit -m \""+commit+"\"")
    os.system("cd "+foldrep+"&&git push origin "+branch)



fileForWork = sys.argv[1]
loginName = sys.argv[3].split('/')[0]
passName = sys.argv[2]
repoName = sys.argv[3].split('/')[1]
folderName = '/tmp/wklee/'
tmpOutBuild = '/tmp/outFile.txt'

branchName = fileForWork.split('/')[-1].split('.c')[0]
commitText = "start work commit"
down_git_branch(loginName,passName,repoName,folderName,branchName)
os.system('rm -rf '+folderName+'*')
os.system('cp '+fileForWork+' '+folderName)
os.system('chmod 777 '+folderName)
os.system('chmod 777 '+folderName+fileForWork.split('/')[-1])
os.system('docker run -t -v '+folderName+':/tmp/code --ulimit=\'stack=-1:-1\' klee/klee:2.1 /usr/bin/clang-6.0 -I /home/klee/klee_src/include -emit-llvm -c -g /tmp/code/'+fileForWork.split('/')[-1]+' -o /tmp/code/'+fileForWork.split('/')[-1]+' 2>&1 > '+tmpOutBuild)
if os.path.exists(folderName+fileForWork.split('/')[-1]):
    os.system('timeout --signal=SIGKILL 18000 docker run -t -v /tmp/wklee/:/tmp/code --ulimit=\'stack=-1:-1\' klee/klee:2.1 /home/klee/klee_build/bin/klee --posix-runtime -libc=uclibc /tmp/code/'+fileForWork.split('/')[-1])
else:
    os.system('cp '+tmpOutBuild+' '+folderName)

save_repo_branch_commit(loginName,passName,repoName,folderName,branchName,"end work commit")
os._exit(0)
