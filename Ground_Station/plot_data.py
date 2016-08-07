import numpy as np
import sys
import matplotlib.pyplot as plt


plt.rcParams['figure.figsize'] =  10, 7.5
plt.rcParams['xtick.labelsize'] = 16
plt.rcParams['ytick.labelsize'] = 16
plt.rcParams['font.sans-serif'] = 'Helvetica'
plt.rcParams['text.color'] = 'black'
plt.rcParams['font.size'] = 18
plt.rcParams['lines.linewidth'] = 2
plt.rcParams['axes.facecolor'] = 'white'
plt.rcParams['axes.edgecolor'] = 'black'
plt.rcParams['axes.labelcolor'] = 'black'
plt.rcParams['axes.axisbelow'] = True
plt.rcParams['xtick.major.size'] = 10
plt.rcParams['xtick.minor.size'] = 5
plt.rcParams['xtick.major.width'] = 1
plt.rcParams['xtick.minor.width'] = 1
plt.rcParams['ytick.major.size'] = 10
plt.rcParams['ytick.minor.size'] = 5
plt.rcParams['ytick.major.width'] = 1
plt.rcParams['ytick.minor.width'] = 1
plt.rcParams['xtick.color'] = 'k'
plt.rcParams['ytick.color'] = 'k'


def calculateDewpoint(temp,rh):
    """
    Calculate dewpoint from known temperature and humidity.
    Modified from: http://andrew.rsmas.miami.edu/bmcnoldy/Humidity.html
    """
    return (243.04 * (np.log(rh / 100.) + ((17.625 * temp) / (243.04 + temp))) /
            (17.625 - np.log(rh / 100) - ((17.625 * temp) / (243.04 + temp))))


def calculateAltitude(P, P0):
    """
    Calculate altitude from barometric pressure readings
    Formula from:
    http://www.mide.com/products/slamstick/air-pressure-altitude-calculator.php
    """
    return 44330.0 * (1 - (P / P0) ** (1 / 5.255))

samples_per_second = 10.

temperatures, pressures, humidities = np.loadtxt(sys.argv[1],
                                                 delimiter=',', unpack=True)
altitudes = calculateAltitude(pressures, np.mean(pressures[0:10]))
dewpoints = calculateDewpoint(temperatures, humidities)
times = np.arange(len(dewpoints))/samples_per_second

# Setup figure and axes
# Generally plots is ~1.33x width to height (10,7.5 or 12,9)
fig = plt.figure(figsize=(18, 8))
ax1 = plt.subplot2grid((2, 3), (0, 0), rowspan=2)
ax2 = plt.subplot2grid((2, 3), (0, 1), colspan=2)
ax3 = plt.subplot2grid((2, 3), (1, 1), colspan=2)
plt.subplots_adjust(hspace=0.3, wspace=0.3)

# Set labels and tick sizes
ax1.set_xlabel(r'Temperature [C]')
ax1.set_ylabel(r'Altitude [m]')

ax2.set_xlabel(r'Time [sec]')
ax2.set_ylabel(r'Altitude [m]')

ax3.set_xlabel(r'Time [sec]')
ax3.set_ylabel(r'Temperature [C]')

# Plotting
ax1.plot(temperatures, altitudes, '-o', color='r')
ax1.plot(dewpoints, altitudes, '-o', color='g')

ax2.plot(times, altitudes, '-o', color='k')

ax3.plot(times, temperatures, '-o', color='r')
ax3.plot(times, dewpoints, '-o', color='g')
plt.show()
