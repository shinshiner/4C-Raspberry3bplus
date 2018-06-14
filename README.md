# 4C-Raspberry3bplus

基于树莓派3b+和出门问问平台的语音识别与分析代码

# Installation

## 外设

* 免驱话筒一个

* 地摊音响一台

* 收音机一架

## 出门问问sdk

[传送门](https://ai.chumenwenwen.com/pages/document/intro?id=download) 或用repo里自带的sdk-1.2.0（.mobvoi/里的文件）

## Python库 (pip or conda)

* itchat

## 电台接口

[传送门]()

# To Run

## 语音助手

```
cd /home/pi/4C-Raspberry3bplus/samples/bin
python3 main.py
```

## 智能电台

在树莓派里放好歌曲文件（.mp3 或 .wav），更新`run.py`里的`play_list`

```
python3 run.py
```

微信扫码登录

把收音机频率调到96.3，或自己在`run.py`里设定新的频率

发信号点歌

```
U i // 放第 i 首歌
P   // 放上一首
N   // 放下一首
S   // 停止播放
```

# 谢三大佬带飞

* [wcx](https://github.com/chenxi-wang)

* [wzr](https://github.com/wzrain)

* [hyx](https://github.com/Riften)