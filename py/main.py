import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
import numpy as np
import dbus, time
from dbus.mainloop.glib import DBusGMainLoop
import BleDevice
from math import *

WINDOW_LEN = 100

dev_uuid     = '00001523-1212-efde-1523-785feabcd123'
rx_char_uuid = '00001524-1212-efde-1523-785feabcd123'
tx_char_uuid = '00001525-1212-efde-1523-785feabcd123'

def prop_changed_cb(iface, changed_props, invalidated_props):
    values = changed_props.get('Value', None)
    if values:
        v = [int(i) for i in values]
        print(v)

        adc = np.int32(((v[0] | (v[1] << 8) | (v[2] << 16) | (v[3] << 24))))
        x = np.int16(v[4] | (v[5] << 8))
        y = np.int16(v[6] | (v[7] << 8))
        z = np.int16(v[8] | (v[9] << 8))

        update_curve(adc, x, y, z)

def update_curve(adc, x, y, z):
    global data, curve

    pitch = degrees(atan(x/sqrt(y**2 + z**2)))
    roll  = degrees(atan(y/sqrt(x**2 + z**2)))
    phi   = degrees(acos(x/sqrt(x**2 + y**2 + z**2)))

    data[0].append(x)
    data[1].append(y)
    data[2].append(z)
    data[3].append(phi)
    data[4].append(adc)

    # roll
    for i in range(len(data)):
        if len(data[i]) > WINDOW_LEN:
            data[i].pop(0)
        curve[i].setData(data[i])

def main():
    DBusGMainLoop(set_as_default=True)
    dev = BleDevice.BleDevice()
    dev.connect(dev_uuid)
    dev.listen(rx_char_uuid, prop_changed_cb)

    #QtGui.QApplication.setGraphicsSystem('opengl')
    app = QtGui.QApplication(['BLE Plotter'])

    win = pg.GraphicsLayoutWidget()
    win.show()

    global curve, data
    data = []
    curve = []

    plt = win.addPlot()
    plt.addLegend()
    plt.showGrid(x=True, y=True)
    curve.append(plt.plot(pen='r', name='x'))
    curve.append(plt.plot(pen='g', name='y'))
    curve.append(plt.plot(pen='b', name='z'))
    data.append([0] * WINDOW_LEN)
    data.append([0] * WINDOW_LEN)
    data.append([0] * WINDOW_LEN)
    plt.setYRange(-32768, 32768, padding=0)

    plt = win.addPlot()
    plt.addLegend()
    plt.showGrid(x=True, y=True)
    curve.append(plt.plot(name='angle'))
    data.append([0] * WINDOW_LEN)

    win.nextRow()

    plt = win.addPlot(colspan=2)
    plt.addLegend()
    plt.showGrid(x=True, y=True, alpha=1.0)
    win.nextRow()
    curve.append(plt.plot(name='force', symbol='o'))
    data.append([0] * WINDOW_LEN)
    #plt.setYRange(-7, 7, padding=0)

    app.exec_()

if __name__ == '__main__':
    main()