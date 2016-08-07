import urllib
import re
import sys
from time import sleep


def get_data_HTTP(link):
    f = urllib.urlopen(link)
    urlreturn = f.read()
    temperature = re.search("Temperature = .*?<br>", urlreturn)
    temperature = temperature.group(0)
    temperature = temperature.split('=')[1]
    temperature = temperature.split('<br>')[0]

    pressure = re.search("Pressure = .*?<br>", urlreturn)
    pressure = pressure.group(0)
    pressure = pressure.split('=')[1]
    pressure = pressure.split('<br>')[0]

    humidity = re.search("Humidity = .*?<br>", urlreturn)
    humidity = humidity.group(0)
    humidity = humidity.split('=')[1]
    humidity = humidity.split('<br>')[0]

    return temperature, pressure, humidity

link = "http://192.168.4.1/read"

keepalive = True

fname = open(sys.argv[1], 'w')
samples_per_second = 10.
while keepalive:
    try:
        # Get new data
        p, t, h = get_data_HTTP(link)
        print(t, p, h)
        fname.write("%s,%s,%s\n" % (p, t, h))

    except:
        pass

    sleep(1./samples_per_second)
fname.close()
