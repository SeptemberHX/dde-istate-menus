## dde-istate-menus

IStat menus on macOS alternative for DDE.

> This software has nothing to do with the Istat menu excepting adopting its UI design

> The main part of the system monitoring code in `utils` comes from [deepin-system-monitor](https://github.com/linuxdeepin/deepin-system-monitor)

> Still in development


* [ ] Network
    * [x] popup widget
    * [ ] dock widget
        * [x] text
        * [ ] chart
* [ ] Weather
* [ ] CPU
* [ ] RAM
* [ ] DISK
* [ ] Datetime
    * [x] dock widget
    * [ ] popup widget
    
### Known issues

* dock widgets' text are really small even the widget has enough space, or text is too large and only part is shown on the widget. I think it is related to the dde-dock [issue](https://github.com/linuxdeepin/dde-dock/issues/271)

### Screenshot

#### Network

![](./screenshots/network.png)


### Attention

```
sudo setcap cap_net_raw,cap_dac_read_search,cap_sys_ptrace+ep `which dde-dock`
```
