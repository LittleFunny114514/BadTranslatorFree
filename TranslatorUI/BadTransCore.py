
import importlib,time
import random as r
import ctypes
from os import system
from sys import argv,stdin,stdout,exit
from types import NoneType
try:
    def install_and_import(package,imp=None):
        if imp==None:imp=package
        try:
            __import__(imp)
        except ImportError:
            import subprocess
            subprocess.check_call(["python", '-m', 'pip', 'install', package])
    install_and_import("googletrans==4.0.0-rc1","googletrans")
    from googletrans import Translator,LANGUAGES

    text=None
    out=stdout
    n=None
    lans=['zh-cn','en','de','fr','ru']#list(LANGUAGES.keys())
    i=1
    Help="""
    参数：
    -Lang All|Default|['lang1','lang2','lang3'] 默认是Default
    -TextIn
    -Lang Default -TextIn file in.txt -TextOut file out.txt -Count 20
    """
    while i<len(argv):
        if argv[i]=='-Help':
            print(Help)
            system("pause")
            exit(0)
        if argv[i]=='-TextIn':
            if argv[i+1]=='file':
                with open(argv[i+2],encoding='utf-8') as file:
                    text=file.read()
                i+=3
            elif argv[i+1]=='std':
                text=input()
                i+=2
            elif argv[i+1]=='command':
                text=argv[i+2]
                i+=3
            else:
                print(f'invalid command:{argv[i+1]}')
                exit(-1)
                i+=1

        elif argv[i]=='-TextOut':
            if argv[i+1]=='file':
                out=open(argv[i+2],'w',encoding='utf-8')
                i+=3
            elif argv[i+1]=='std':
                out=stdout
                i+=2
            else:
                print(f'invalid command:{argv[i+1]}')
                exit(-1)
                i+=1
        elif argv[i]=='-Lang':
            All=list(LANGUAGES.keys())
            Default=['zh-cn','en','de','fr','ru']
            lans=eval(argv[i+1])
            i+=2
        elif argv[i]=='-Count':
            n=int(argv[i+1])
            i+=2
        else:
            print(f'invalid command:{argv[i]}')
            exit(-1)
    if text == None:text=input()
    if n==None:n=int(input())
    translator = Translator(service_urls=['translate.google.com'])
    def trans(text,ddest,ffrom='auto'):
        #print(ddest,ffrom)
        try:
            return translator.translate(text, dest=ddest,src=ffrom).text
        except Exception as e:
            print(type(e),e)
            __import__("time").sleep(1)
            return trans(text,ddest)
    def print_progress_bar(percent, remaining_time):
        bar_length = 80
        filled_length = int(percent / 100 * bar_length)
        remaining_length = bar_length - filled_length
        bar = '[' + '=' * filled_length + ' ' * remaining_length + ']'
        print('{} {:>3}% {:>5}:{}:{}      '.format(bar, round(percent,2), remaining_time//3600,remaining_time%3600//60,remaining_time%60), end='\r')


    start_time = time.time()
    lastlang='auto'
    for i in range(n):
        elapsed_time = time.time() - start_time  # 已用时间（秒）
        progress_ratio = max((i) / (n),0.00001)  # 已完成比例
        remaining_time = (1 - progress_ratio) * elapsed_time / progress_ratio  # 剩余时间（秒）
        print_progress_bar((100*i)/(n),int(remaining_time))
        lan=r.choice(lans)
        if i==(n-1):lan='zh-CN'
        text=trans(text,lan,lastlang)
        lastlang=lan
    system("cls")
    #text=trans(text,'zh-CN')
    for c in text:
        try:
            print(c,file=out,end='')
        except UnicodeEncodeError:
            print('?',file=out,end='')
    if out==stdout:
        while True:time.sleep(10000)
    else:out.close()
except BaseException as e:
    print(type(e),e)
    print(argv)
    while True:time.sleep(10000)