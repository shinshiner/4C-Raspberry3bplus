#coding=utf-8
# from WxNeteaseMusic import WxNeteaseMusic
import itchat
import os
import threading
import subprocess

# wnm = WxNeteaseMusic()
# @itchat.msg_register(itchat.content.TEXT)
# def mp3_player(msg):
#     text = msg['Text']
#     res = wnm.msg_handler(text)
#     return res

curr_idx = 0
playlist = [
    '/home/pi/speechsdk-1.2.0/samples/bin/mix_tts_6.wav',
    '/home/pi/speechsdk-1.2.0/samples/bin/mix_tts_3.wav',
    '/home/pi/speechsdk-1.2.0/samples/bin/mix_tts_0.wav',
    '/home/pi/NewsDictation01.mp3',
    '/home/pi/empty.wav',
]

def play(idx=0):
    # os.system('sudo pi_fm_rds -freq 96.3 -audio %s' % playlist[idx])
    suffix = playlist[idx].split('.')[-1]
    if suffix == 'wav':
        os.system('sudo pi_fm_rds -freq 96.3 -audio %s' % playlist[idx])
    elif suffix == 'mp3':
        os.system('sox -t mp3 %s -t wav -  | sudo pi_fm_rds -freq 96.3 -audio -' % (playlist[idx]))

def kill_fm():
    os.system('ps aux | grep pi_fm |grep -v grep| cut -c 9-15 | xargs kill -9')

@itchat.msg_register(itchat.content.TEXT)
def cmd(msg):
    texts = msg['Text'].split()
    if texts[0] == 'U':
        global curr_idx 
        curr_idx = int(texts[1])
        print("playing song: " + playlist[curr_idx])
        kill_fm()
        t = threading.Thread(target=play, args=(curr_idx,))
        t.start()
        #subprocess.Popen('sudo pi_fm_rds -freq 96.3 -audio %s' % playlist[idx], shell=True, stdout=subprocess.PIPE)
        #play(curr_idx)
    elif texts[0] == 'N':
        global curr_idx 
        curr_idx += 1
        curr_idx %= len(playlist)
        print("playing song: " + playlist[curr_idx])
        kill_fm()
        t = threading.Thread(target=play, args=(curr_idx,))
        t.start()
        #subprocess.Popen('sudo pi_fm_rds -freq 96.3 -audio %s' % playlist[idx], shell=True, stdout=subprocess.PIPE)
        #play(curr_idx)
    elif texts[0] == 'P':
        global curr_idx
        curr_idx -= 1
        if curr_idx < 0:
            curr_idx = len(playlist) - 1
        print("playing song: " + playlist[curr_idx])
        kill_fm()
        t = threading.Thread(target=play, args=(curr_idx,))
        t.start()
        #subprocess.Popen('sudo pi_fm_rds -freq 96.3 -audio %s' % playlist[idx], shell=True, stdout=subprocess.PIPE)
    elif texts[0] == 'S':
        kill_fm()
        t = threading.Thread(target=play, args=(-1,))
        t.start()

@itchat.msg_register(itchat.content.RECORDING)
def on_voice(msg):
	msg['Text']('/home/pi/wechat_voice.mp3')


itchat.auto_login(enableCmdQR=2)
itchat.run(debug=True)
