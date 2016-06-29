import urllib
from time import sleep

link = "http://192.168.4.1/read"

for i in range (20):
    f = urllib.urlopen(link)
    urlreturn = f.read()
    print urlreturn
    sleep(0.5)
