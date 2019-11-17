import dbus, time

class BleDevice:
    def __init__(self):
        self.bus = dbus.SystemBus()

    def set_discovery_state(self, state, adapter_name='hci0'):
        hci = self.bus.get_object('org.bluez', '/org/bluez/' + adapter_name)
        adapter = dbus.Interface(hci, dbus_interface='org.bluez.Adapter1')
        try:
            if state == True:
                adapter.StartDiscovery()
            else:
                adapter.StopDiscovery()
        except dbus.exceptions.DBusException as ex:
            print(ex)


    def get_proxy_by_uuid(self, char_uuid, iface, props):
        obj_manager = dbus.Interface(self.bus.get_object('org.bluez', '/'), 'org.freedesktop.DBus.ObjectManager')

        for path in obj_manager.GetManagedObjects():
            dev_proxy = self.bus.get_object('org.bluez', path)
            try:
                dev_prop = dbus.Interface(dev_proxy, dbus_interface='org.freedesktop.DBus.Properties')
                uuids = dev_prop.Get(iface, props, dbus_interface='org.freedesktop.DBus.Properties')
                if char_uuid in uuids:
                    return dev_proxy
            except:
                continue
        return None

    def connect(self, dev_uuid, timeout=10):
        self.set_discovery_state(True)

        print('Searching for device.. ', end='')
        dev_proxy = None
        for i in range(timeout*10):
            dev_proxy = self.get_proxy_by_uuid(dev_uuid, 'org.bluez.Device1', 'UUIDs')
            if dev_proxy:
                break
            time.sleep(0.1)
        else:
            print('failed!')
            return None
        print('found.')

        self.set_discovery_state(False)

        print('Connecting to device.. ', end='')
        dev_prop = dbus.Interface(dev_proxy, dbus_interface='org.freedesktop.DBus.Properties')
        dev = dbus.Interface(dev_proxy, dbus_interface='org.bluez.Device1')
        dev.Connect()
        for i in range(timeout*10):
            if dev_prop.Get('org.bluez.Device1', 'ServicesResolved',
                            dbus_interface='org.freedesktop.DBus.Properties') == True:
                            break
            time.sleep(0.1)
        else:
            print('failed!')
            return None
        print('done.')

    def __error_cb(self, err):
        print('D-Bus call failed: ' + str(err))

    def listen(self, char_uuid, prop_changed_cb):
        """ callback function: prop_changed_cb(iface, changed_props, invalidated_props) """
        # get charecteristic
        proxy = self.get_proxy_by_uuid(char_uuid, 'org.bluez.GattCharacteristic1', 'UUID')

        # connect to PropertiesChanged signal
        prop = dbus.Interface(proxy, dbus_interface='org.freedesktop.DBus.Properties')
        prop.connect_to_signal('PropertiesChanged', prop_changed_cb)

        # start notifications
        chrc = dbus.Interface(proxy, dbus_interface='org.bluez.GattCharacteristic1')
        chrc.StartNotify(reply_handler=lambda:None, error_handler=self.__error_cb, dbus_interface='org.bluez.GattCharacteristic1')
